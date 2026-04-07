// Tencent is pleased to support the open source community by making UnLua available.
//
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the MIT License (the "License");
// you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EpicGames.Core;
using EpicGames.UHT.Tables;
using EpicGames.UHT.Types;
using EpicGames.UHT.Utils;

namespace UnLuaDefaultParamCollector
{
	[UnrealHeaderTool]
	internal class UnLuaDefaultParamExporter
	{
		[UhtExporter(Name = "UnLuaDefaultParams", ModuleName = "UnLua",
			Description = "UnLua UFUNCTION default parameter collector",
			Options = UhtExporterOptions.Default,
			OtherFilters = ["DefaultParamCollection.inl"])]
		public static void Export(IUhtExportFactory factory)
		{
			new UnLuaDefaultParamExporter(factory).Generate();
		}

		private readonly IUhtExportFactory _factory;
		private UhtSession Session => _factory.Session;

		private UnLuaDefaultParamExporter(IUhtExportFactory factory)
		{
			_factory = factory;
		}

		private void Generate()
		{
			// Collect all classes from all modules
			List<Task?> tasks = new();
			Dictionary<string, StringBuilder> classBuilders = new();
			object lockObj = new();

			foreach (UhtModule module in Session.Modules)
			{
				if (module.Module.ModuleType != UHTModuleType.EngineRuntime &&
					module.Module.ModuleType != UHTModuleType.GameRuntime)
				{
					continue;
				}

				CollectClasses(module.ScriptPackage, tasks, classBuilders, lockObj);
			}

			// Wait for all tasks
			Task[] waitTasks = tasks.Where(x => x != null).Cast<Task>().ToArray();
			if (waitTasks.Length > 0)
			{
				Task.WaitAll(waitTasks);
			}

			// Build final output
			StringBuilder output = new();
			AppendSharedValues(output);

			// Sort class names for deterministic output
			List<string> sortedClassNames = new(classBuilders.Keys);
			sortedClassNames.Sort(StringComparer.Ordinal);

			foreach (string className in sortedClassNames)
			{
				StringBuilder classBuilder = classBuilders[className];
				if (classBuilder.Length > 0)
				{
					output.AppendLine($"FC = &GDefaultParamCollection.Add(TEXT(\"{className}\"));");
					output.Append(classBuilder);
					output.AppendLine();
				}
			}

			string filePath = _factory.MakePath("DefaultParamCollection", ".inl");
			_factory.CommitOutput(filePath, output);
		}

		private static void AppendSharedValues(StringBuilder sb)
		{
			sb.AppendLine("FFunctionCollection* FC = nullptr;");
			sb.AppendLine("FParameterCollection* PC = nullptr;");
			sb.AppendLine();
			sb.AppendLine("FBoolParamValue* SharedBool_TRUE = new FBoolParamValue(true);");
			sb.AppendLine("FBoolParamValue* SharedBool_FALSE = new FBoolParamValue(false);");
			sb.AppendLine("FFloatParamValue* SharedFloat_Zero = new FFloatParamValue(0.000000f);");
			sb.AppendLine("FFloatParamValue* SharedFloat_One = new FFloatParamValue(1.000000f);");
			sb.AppendLine("FEnumParamValue* SharedEnum_Zero = new FEnumParamValue(0);");
			sb.AppendLine("FIntParamValue* SharedInt_Zero = new FIntParamValue(0);");
			sb.AppendLine("FByteParamValue* SharedByte_Zero = new FByteParamValue(0);");
			sb.AppendLine("FNameParamValue* SharedFName_None = new FNameParamValue(FName(\"None\"));");
			sb.AppendLine();
			sb.AppendLine("FVectorParamValue* SharedFVector_Zero = new FVectorParamValue(FVector(EForceInit::ForceInitToZero));");
			sb.AppendLine("FVector2DParamValue* SharedFVector2D_Zero = new FVector2DParamValue(FVector2D(EForceInit::ForceInitToZero));");
			sb.AppendLine("FRotatorParamValue* SharedFRotator_Zero = new FRotatorParamValue(FRotator(EForceInit::ForceInitToZero));");
			sb.AppendLine("FLinearColorParamValue* SharedFLinearColor_Zero = new FLinearColorParamValue(FLinearColor(EForceInit::ForceInitToZero));");
			sb.AppendLine("FColorParamValue* SharedFColor_Zero = new FColorParamValue(FColor(EForceInit::ForceInitToZero));");
			sb.AppendLine();
			sb.AppendLine("FScriptArrayParamValue* SharedScriptArray = new FScriptArrayParamValue();");
			sb.AppendLine("FScriptDelegateParamValue* SharedScriptDelegate = new FScriptDelegateParamValue(FScriptDelegate());");
			sb.AppendLine("FMulticastScriptDelegateParamValue* SharedMulticastScriptDelegate = new FMulticastScriptDelegateParamValue(FMulticastScriptDelegate());");
			sb.AppendLine();
		}

		private void CollectClasses(UhtType type, List<Task?> tasks, Dictionary<string, StringBuilder> classBuilders, object lockObj)
		{
			if (type is UhtClass classObj)
			{
				// Skip interfaces
				if (!classObj.ClassFlags.HasAnyFlags(EClassFlags.Interface))
				{
					tasks.Add(_factory.CreateTask(_ =>
					{
						StringBuilder? classBuilder = ExportClass(classObj);
						if (classBuilder != null)
						{
							lock (lockObj)
							{
								classBuilders[classObj.SourceName] = classBuilder;
							}
						}
					}));
				}
			}

			foreach (UhtType child in type.Children)
			{
				CollectClasses(child, tasks, classBuilders, lockObj);
			}
		}

		private static StringBuilder? ExportClass(UhtClass classObj)
		{
			StringBuilder? result = null;

			foreach (UhtType child in classObj.Children)
			{
				if (child is UhtFunction function)
				{
					StringBuilder? funcBuilder = ExportFunction(function);
					if (funcBuilder != null)
					{
						result ??= new StringBuilder();
						result.AppendLine($"PC = &FC->Functions.Add(TEXT(\"{function.SourceName}\"));");
						result.Append(funcBuilder);
					}
				}
			}

			return result;
		}

		private static StringBuilder? ExportFunction(UhtFunction function)
		{
			// Get AutoCreateRefTerm parameter names
			HashSet<string> autoEmitParams = new(StringComparer.Ordinal);
			if (function.MetaData.TryGetValue("AutoCreateRefTerm", out string? autoCreateRefTerm) && !String.IsNullOrEmpty(autoCreateRefTerm))
			{
				foreach (string paramName in autoCreateRefTerm.Split(','))
				{
					autoEmitParams.Add(paramName.Trim());
				}
			}

			StringBuilder? result = null;

			foreach (UhtType child in function.Children)
			{
				if (child is not UhtProperty property)
				{
					continue;
				}

				// Skip return params
				if (property.PropertyFlags.HasAnyFlags(EPropertyFlags.ReturnParm))
				{
					continue;
				}

				// Skip non-parameter properties
				if (!property.PropertyFlags.HasAnyFlags(EPropertyFlags.Parm))
				{
					continue;
				}

				// Try to find default value from metadata
				if (!TryGetDefaultValue(function, property, out string? valueStr))
				{
					if (!autoEmitParams.Contains(property.SourceName))
					{
						continue;
					}
					valueStr = ""; // AutoCreateRefTerm param with no explicit default
				}

				if (valueStr != null)
				{
					string? paramCode = GenerateParamCode(property, valueStr);
					if (paramCode != null)
					{
						result ??= new StringBuilder();
						result.AppendLine(paramCode);
					}
				}
			}

			return result;
		}

		private static bool TryGetDefaultValue(UhtFunction function, UhtProperty property, out string? value)
		{
			// Check CPP_Default_<ParamName>
			string cppKey = $"CPP_Default_{property.SourceName}";
			if (function.MetaData.TryGetValue(cppKey, out value))
			{
				return true;
			}

			// Check direct param name
			if (function.MetaData.TryGetValue(property.SourceName, out value))
			{
				return true;
			}

			value = null;
			return false;
		}

		private static string? GenerateParamCode(UhtProperty property, string valueStr)
		{
			string paramName = property.SourceName;

			switch (property)
			{
				case UhtStructProperty structProp:
					return GenerateStructParamCode(paramName, structProp, valueStr);

				case UhtIntProperty:
				{
					int value = ParseInt(valueStr);
					return value == 0
						? $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedInt_Zero);"
						: $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FIntParamValue({value}));";
				}

				case UhtByteProperty byteProp:
					return GenerateByteParamCode(paramName, byteProp, valueStr);

				case UhtEnumProperty enumProp:
					return GenerateEnumParamCode(paramName, enumProp, valueStr);

				case UhtFloatProperty:
				{
					float value = ParseFloat(valueStr);
					if (MathF.Abs(value) < 1e-6f)
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFloat_Zero);";
					else if (MathF.Abs(value - 1.0f) < 1e-6f)
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFloat_One);";
					else
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FFloatParamValue({FormatFloat(value)}f));";
				}

				case UhtDoubleProperty:
				{
					double value = ParseDouble(valueStr);
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FDoubleParamValue({FormatDouble(value)}));";
				}

				case UhtBoolProperty:
				{
					bool value = ParseBool(valueStr);
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedBool_{(value ? "TRUE" : "FALSE")});";
				}

				case UhtNameProperty:
				{
					if (String.IsNullOrEmpty(valueStr) || valueStr == "None")
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFName_None);";
					else
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FNameParamValue(FName(\"{valueStr}\")));";
				}

				case UhtTextProperty:
				{
					if (valueStr.StartsWith("INVTEXT(\""))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FTextParamValue({valueStr}));";
					else
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FTextParamValue(FText::FromString(TEXT(\"{valueStr}\"))));";
				}

				case UhtStrProperty:
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FStringParamValue(TEXT(\"{valueStr}\")));";

				case UhtArrayProperty:
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedScriptArray);";

				case UhtDelegateProperty:
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedScriptDelegate);";

				case UhtMulticastDelegateProperty:
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedMulticastScriptDelegate);";

				default:
					return null;
			}
		}

		private static string? GenerateStructParamCode(string paramName, UhtStructProperty structProp, string valueStr)
		{
			string structName = structProp.ScriptStruct.SourceName;

			switch (structName)
			{
				case "FVector":
				{
					if (String.IsNullOrEmpty(valueStr))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFVector_Zero);";
					string[] parts = valueStr.Split(',');
					if (parts.Length == 3)
					{
						float x = ParseFloat(parts[0]), y = ParseFloat(parts[1]), z = ParseFloat(parts[2]);
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FVectorParamValue(FVector({FormatFloat(x)}f,{FormatFloat(y)}f,{FormatFloat(z)}f)));";
					}
					return null;
				}

				case "FRotator":
				{
					if (String.IsNullOrEmpty(valueStr))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFRotator_Zero);";
					string[] parts = valueStr.Split(',');
					if (parts.Length == 3)
					{
						float p = ParseFloat(parts[0]), y = ParseFloat(parts[1]), r = ParseFloat(parts[2]);
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FRotatorParamValue(FRotator({FormatFloat(p)}f,{FormatFloat(y)}f,{FormatFloat(r)}f)));";
					}
					return null;
				}

				case "FVector2D":
				{
					if (String.IsNullOrEmpty(valueStr))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFVector2D_Zero);";
					// Try to parse (X=...,Y=...) format
					if (TryParseVector2D(valueStr, out float vx, out float vy))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FVector2DParamValue(FVector2D({FormatFloat(vx)}f,{FormatFloat(vy)}f)));";
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFVector2D_Zero);";
				}

				case "FLinearColor":
				{
					if (String.IsNullOrEmpty(valueStr))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFLinearColor_Zero);";
					if (TryParseLinearColor(valueStr, out float cr, out float cg, out float cb, out float ca))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FLinearColorParamValue(FLinearColor({FormatFloat(cr)}f,{FormatFloat(cg)}f,{FormatFloat(cb)}f,{FormatFloat(ca)}f)));";
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFLinearColor_Zero);";
				}

				case "FColor":
				{
					if (String.IsNullOrEmpty(valueStr))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFColor_Zero);";
					if (TryParseColor(valueStr, out int colR, out int colG, out int colB, out int colA))
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FColorParamValue(FColor({colR},{colG},{colB},{colA})));";
					return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedFColor_Zero);";
				}

				default:
					return null; // Unsupported struct type
			}
		}

		private static string GenerateByteParamCode(string paramName, UhtByteProperty byteProp, string valueStr)
		{
			UhtEnum? enumObj = byteProp.Enum;
			if (enumObj != null && !String.IsNullOrEmpty(valueStr))
			{
				// Try to find the enum value by name
				int enumIndex = FindEnumIndexByName(enumObj, valueStr);
				if (enumIndex >= 0)
				{
					long enumValue = GetEnumValueByIndex(enumObj, enumIndex);
					if (enumValue == 0)
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedByte_Zero);";
					else if (enumValue > 0 && enumValue <= 255)
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FByteParamValue({enumValue}));";
					else
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FRuntimeEnumParamValue(\"{enumObj.CppType}\",{enumIndex}));";
				}
			}

			int byteValue = ParseInt(valueStr);
			return byteValue == 0
				? $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedByte_Zero);"
				: $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FByteParamValue({byteValue}));";
		}

		private static string GenerateEnumParamCode(string paramName, UhtEnumProperty enumProp, string valueStr)
		{
			UhtEnum enumObj = enumProp.Enum;
			if (!String.IsNullOrEmpty(valueStr))
			{
				int enumIndex = FindEnumIndexByName(enumObj, valueStr);
				if (enumIndex >= 0)
				{
					long enumValue = GetEnumValueByIndex(enumObj, enumIndex);
					if (enumValue == 0)
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedEnum_Zero);";
					else if (enumValue > 0 && enumValue <= 255)
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FEnumParamValue({enumValue}));";
					else
						return $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FRuntimeEnumParamValue(\"{enumObj.CppType}\",{enumIndex}));";
				}
			}

			long value = ParseLong(valueStr);
			return value == 0
				? $"PC->Parameters.Add(TEXT(\"{paramName}\"), SharedEnum_Zero);"
				: $"PC->Parameters.Add(TEXT(\"{paramName}\"), new FEnumParamValue({value}));";
		}

		private static int FindEnumIndexByName(UhtEnum enumObj, string name)
		{
			// Search through enum values
			for (int i = 0; i < enumObj.EnumValues.Count; i++)
			{
				if (String.Equals(enumObj.EnumValues[i].Name, name, StringComparison.Ordinal))
				{
					return i;
				}
				// Also check short name (without enum prefix)
				string fullName = $"{enumObj.SourceName}::{name}";
				if (String.Equals(enumObj.EnumValues[i].Name, fullName, StringComparison.Ordinal))
				{
					return i;
				}
			}
			return -1;
		}

		private static long GetEnumValueByIndex(UhtEnum enumObj, int index)
		{
			if (index >= 0 && index < enumObj.EnumValues.Count)
			{
				return enumObj.EnumValues[index].Value;
			}
			return 0;
		}

		// --- Parsing helpers ---

		private static int ParseInt(string s)
		{
			if (String.IsNullOrEmpty(s)) return 0;
			if (Int32.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, out int result))
				return result;
			return 0;
		}

		private static long ParseLong(string s)
		{
			if (String.IsNullOrEmpty(s)) return 0;
			if (Int64.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, out long result))
				return result;
			return 0;
		}

		private static float ParseFloat(string s)
		{
			if (String.IsNullOrEmpty(s)) return 0.0f;
			if (Single.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, out float result))
				return result;
			return 0.0f;
		}

		private static double ParseDouble(string s)
		{
			if (String.IsNullOrEmpty(s)) return 0.0;
			if (Double.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, out double result))
				return result;
			return 0.0;
		}

		private static bool ParseBool(string s)
		{
			if (String.IsNullOrEmpty(s)) return false;
			return String.Equals(s, "true", StringComparison.OrdinalIgnoreCase);
		}

		private static string FormatFloat(float v) => v.ToString("F6", CultureInfo.InvariantCulture);
		private static string FormatDouble(double v) => v.ToString("F6", CultureInfo.InvariantCulture);

		// Parse FVector2D from "(X=...,Y=...)" format
		private static bool TryParseVector2D(string s, out float x, out float y)
		{
			x = 0; y = 0;
			Dictionary<string, string>? kv = ParseKeyValuePairs(s);
			if (kv != null && kv.TryGetValue("X", out string? xs) && kv.TryGetValue("Y", out string? ys))
			{
				x = ParseFloat(xs);
				y = ParseFloat(ys);
				return true;
			}
			return false;
		}

		// Parse FLinearColor from "(R=...,G=...,B=...,A=...)" format
		private static bool TryParseLinearColor(string s, out float r, out float g, out float b, out float a)
		{
			r = 0; g = 0; b = 0; a = 0;
			Dictionary<string, string>? kv = ParseKeyValuePairs(s);
			if (kv != null && kv.TryGetValue("R", out string? rs) && kv.TryGetValue("G", out string? gs) &&
				kv.TryGetValue("B", out string? bs) && kv.TryGetValue("A", out string? aStr))
			{
				r = ParseFloat(rs);
				g = ParseFloat(gs);
				b = ParseFloat(bs);
				a = ParseFloat(aStr);
				return true;
			}
			return false;
		}

		// Parse FColor from "(R=...,G=...,B=...,A=...)" format (integer components 0-255)
		private static bool TryParseColor(string s, out int r, out int g, out int b, out int a)
		{
			r = 0; g = 0; b = 0; a = 0;
			Dictionary<string, string>? kv = ParseKeyValuePairs(s);
			if (kv != null && kv.TryGetValue("R", out string? rs) && kv.TryGetValue("G", out string? gs) &&
				kv.TryGetValue("B", out string? bs) && kv.TryGetValue("A", out string? aStr))
			{
				r = ParseInt(rs);
				g = ParseInt(gs);
				b = ParseInt(bs);
				a = ParseInt(aStr);
				return true;
			}
			return false;
		}

		// Parse "(Key1=Value1,Key2=Value2,...)" format into a dictionary
		private static Dictionary<string, string>? ParseKeyValuePairs(string s)
		{
			s = s.Trim();
			if (s.StartsWith('(') && s.EndsWith(')'))
			{
				s = s[1..^1];
			}
			string[] pairs = s.Split(',');
			Dictionary<string, string> result = new(StringComparer.OrdinalIgnoreCase);
			foreach (string pair in pairs)
			{
				int eq = pair.IndexOf('=');
				if (eq > 0)
				{
					string key = pair[..eq].Trim();
					string val = pair[(eq + 1)..].Trim();
					result[key] = val;
				}
			}
			return result.Count > 0 ? result : null;
		}
	}
}

