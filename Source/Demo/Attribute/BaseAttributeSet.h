// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Component/DAbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"

UCLASS()
class DEMO_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	ATTRIBUTE_ACCESSORS(ThisClass, Hp)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHp)
	ATTRIBUTE_ACCESSORS(ThisClass, HpRegen)
	ATTRIBUTE_ACCESSORS(ThisClass, Mp)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMp)
	ATTRIBUTE_ACCESSORS(ThisClass, MpRegen)
	ATTRIBUTE_ACCESSORS(ThisClass, Lv)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxLv)
	ATTRIBUTE_ACCESSORS(ThisClass, Damage)
	ATTRIBUTE_ACCESSORS(ThisClass, ExtraDamage)
	ATTRIBUTE_ACCESSORS(ThisClass, Armor)
	ATTRIBUTE_ACCESSORS(ThisClass, ExtraArmor)
	ATTRIBUTE_ACCESSORS(ThisClass, MagicResist)
	ATTRIBUTE_ACCESSORS(ThisClass, ExtraMagicResist)
	ATTRIBUTE_ACCESSORS(ThisClass, Evasion)
	ATTRIBUTE_ACCESSORS(ThisClass, AttackSpeed)

protected:
	UFUNCTION()
	void OnRep_Hp(const FGameplayAttributeData& OldHp) const;

	UFUNCTION()
	void OnRep_MaxHp(const FGameplayAttributeData& OldHpMax) const;

	UFUNCTION()
	void OnRep_HpRegen(const FGameplayAttributeData& OldHpRegen) const;

	UFUNCTION()
	void OnRep_Mp(const FGameplayAttributeData& OldMp) const;

	UFUNCTION()
	void OnRep_MaxMp(const FGameplayAttributeData& OldMpMax) const;

	UFUNCTION()
	void OnRep_MpRegen(const FGameplayAttributeData& OldMpRegen) const;

	UFUNCTION()
	void OnRep_Lv(const FGameplayAttributeData& OldLv) const;

	UFUNCTION()
	void OnRep_MaxLv(const FGameplayAttributeData& OldLvMax) const;

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldDamage) const;
	
	UFUNCTION()
	void OnRep_ExtraDamage(const FGameplayAttributeData& OldExtraDamage) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_ExtraArmor(const FGameplayAttributeData& OldExtraArmor) const;

	UFUNCTION()
	void OnRep_MagicResist(const FGameplayAttributeData& OldMagicResist) const;

	UFUNCTION()
	void OnRep_ExtraMagicResist(const FGameplayAttributeData& OldExtraMagicResist) const;

	UFUNCTION()
	void OnRep_Evasion(const FGameplayAttributeData& OldEvasion) const;

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Hp)
	FGameplayAttributeData Hp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHp)
	FGameplayAttributeData MaxHp;

	UPROPERTY(ReplicatedUsing = OnRep_HpRegen)
	FGameplayAttributeData HpRegen;

	UPROPERTY(ReplicatedUsing = OnRep_Mp)
	FGameplayAttributeData Mp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxMp)
	FGameplayAttributeData MaxMp;

	UPROPERTY(ReplicatedUsing = OnRep_MpRegen)
	FGameplayAttributeData MpRegen;

	UPROPERTY(ReplicatedUsing = OnRep_Lv)
	FGameplayAttributeData Lv;

	UPROPERTY(ReplicatedUsing = OnRep_MaxLv)
	FGameplayAttributeData MaxLv;

	UPROPERTY(REplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;

	UPROPERTY(REplicatedUsing = OnRep_ExtraDamage)
	FGameplayAttributeData ExtraDamage;

	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;

	UPROPERTY(ReplicatedUsing = OnRep_ExtraArmor)
	FGameplayAttributeData ExtraArmor;

	UPROPERTY(ReplicatedUsing = OnRep_MagicResist)
	FGameplayAttributeData MagicResist;
	
	UPROPERTY(ReplicatedUsing = OnRep_ExtraMagicResist)
	FGameplayAttributeData ExtraMagicResist;

	UPROPERTY(ReplicatedUsing = OnRep_Evasion)
	FGameplayAttributeData Evasion;

	UPROPERTY(ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
};