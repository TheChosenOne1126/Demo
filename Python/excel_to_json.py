import pandas as pd
import os
import tkinter as tk
from tkinter import filedialog, messagebox

def export_selected_excels_to_json():
    file_paths = filedialog.askopenfilenames(
        title="选择 Excel 文件",
        filetypes=[("Excel files", "*.xlsx *.xls")]
    )
    if not file_paths:
        messagebox.showinfo("提示", "未选择任何文件。")
        return

    output_dir = "json_outputs"
    os.makedirs(output_dir, exist_ok=True)

    for file_path in file_paths:
        filename = os.path.basename(file_path)
        base_name, ext = os.path.splitext(filename)
        engine = 'openpyxl' if ext == '.xlsx' else 'xlrd'

        try:
            xls = pd.read_excel(file_path, sheet_name=None, engine=engine)
            for sheet_name, df in xls.items():
                json_file = os.path.join(output_dir, f"{base_name}_{sheet_name}.json")
                df.to_json(json_file, orient='records', force_ascii=False)
            print(f"✅ 成功导出: {filename}")
        except Exception as e:
            print(f"❌ 错误处理文件 {filename}: {e}")

    messagebox.showinfo("完成", f"所有选中的 Excel 文件已导出为 JSON，保存在目录：{output_dir}")

# 创建图形界面
root = tk.Tk()
root.title("Excel 转 JSON 工具")
root.geometry("300x150")

label = tk.Label(root, text="选择 Excel 文件并导出为 JSON", pady=20)
label.pack()

export_button = tk.Button(root, text="选择文件并导出", command=export_selected_excels_to_json)
export_button.pack(pady=10)

root.mainloop()
