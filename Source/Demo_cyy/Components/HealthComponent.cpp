#include "Components/HealthComponent.h"

#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	// ���Ĭ�����ø��ƣ���֤�ͻ���Ҳ���õ�Ѫ��״̬��
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// ����������ó�ʼֵ������ͻ��˸���Ȩ��״̬��
		ResetHealth();
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}

float UHealthComponent::ApplyDamage(float DamageAmount, AActor* InstigatorActor)
{
	// ������˸�Ѫ���ͻ���ֻ���ո��ƽ����
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return CurrentHealth;
	}

	if (bIsDead || DamageAmount <= 0.0f)
	{
		return CurrentHealth;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, GetMaxHealth());
	const float Delta = CurrentHealth - OldHealth;

	// ������ȹ㲥���������ر�������־��
	OnHealthChanged.Broadcast(this, CurrentHealth, Delta, InstigatorActor);

	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast(GetOwner());
	}

	return CurrentHealth;
}

float UHealthComponent::Heal(float HealAmount, AActor* InstigatorActor)
{
	// ������˸�Ѫ���ͻ���ֻ���ո��ƽ����
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return CurrentHealth;
	}

	if (bIsDead || HealAmount <= 0.0f)
	{
		return CurrentHealth;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, GetMaxHealth());
	const float Delta = CurrentHealth - OldHealth;

	if (!FMath::IsNearlyZero(Delta))
	{
		OnHealthChanged.Broadcast(this, CurrentHealth, Delta, InstigatorActor);
	}

	return CurrentHealth;
}

void UHealthComponent::ResetHealth()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	bIsDead = false;
	CurrentHealth = GetMaxHealth();
	OnHealthChanged.Broadcast(this, CurrentHealth, 0.0f, nullptr);
}

void UHealthComponent::OnRep_CurrentHealth(float OldHealth)
{
	// �ͻ����յ����ƺ󲹷�ͳһ�¼���UI ����Ҫ������Դ��
	const float Delta = CurrentHealth - OldHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, Delta, nullptr);
}

void UHealthComponent::OnRep_IsDead()
{
	if (bIsDead)
	{
		OnDeath.Broadcast(GetOwner());
	}
}

