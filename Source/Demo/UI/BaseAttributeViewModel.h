// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "BaseAttributeViewModel.generated.h"

UCLASS(meta = (MVVMAllowedContextCreationType = PropertyPath))
class DEMO_API UBaseAttributeViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	float GetHp() const;

	void SetHp(float NewHp);

	float GetMaxHp() const;

	void SetMaxHp(float NewMaxHp);

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetHpPercentage() const;
	
	float GetHpRegen() const;

	void SetHpRegen(float NewHpRegen);

	float GetMp() const;

	void SetMp(float NewMp);

	float GetMaxMp() const;

	void SetMaxMp(float NewMaxMp);

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetMpPercentage() const;

	float GetMpRegen() const;

	void SetMpRegen(float NewMpRegen);
	
	float GetLv() const;

	void SetLv(float NewLv);
	
	float GetDamage() const;

	void SetDamage(float NewDamage);
	
	float GetExtraDamage() const;

	void SetExtraDamage(float NewExtraDamage);

	float GetArmor() const;

	void SetArmor(float NewArmor);

	float GetExtraArmor() const;

	void SetExtraArmor(float NewExtraArmor);

	float GetMagicResist() const;

	void SetMagicResist(float NewMagicResist);

	float GetExtraMagicResist() const;

	void SetExtraMagicResist(float NewExtraMagicResist);

	float GetEvasion() const;

	void SetEvasion(float NewEvasion);
	
	float GetAttackSpeed() const;

	void SetAttackSpeed(float NewAttackSpeed);

private:
	UPROPERTY(FieldNotify, Setter, Getter)
	float Hp;

	UPROPERTY(FieldNotify, Setter, Getter)
	float MaxHp;

	UPROPERTY(FieldNotify, Setter, Getter)
	float HpRegen;

	UPROPERTY(FieldNotify, Setter, Getter)
	float Mp;

	UPROPERTY(FieldNotify, Setter, Getter)
	float MaxMp;

	UPROPERTY(FieldNotify, Setter, Getter)
	float MpRegen;

	UPROPERTY(FieldNotify, Setter, Getter)
	float Lv;
	
	UPROPERTY(FieldNotify, Setter, Getter)
	float Damage;
	
	UPROPERTY(FieldNotify, Setter, Getter)
	float ExtraDamage;

	UPROPERTY(FieldNotify, Setter, Getter)
	float Armor;

	UPROPERTY(FieldNotify, Setter, Getter)
	float ExtraArmor;

	UPROPERTY(FieldNotify, Setter, Getter)
	float MagicResist;
	
	UPROPERTY(FieldNotify, Setter, Getter)
	float ExtraMagicResist;

	UPROPERTY(FieldNotify, Setter, Getter)
	float Evasion;

	UPROPERTY(FieldNotify, Setter, Getter)
	float AttackSpeed;
};