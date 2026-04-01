#include "UI/DeflectCooldownWidget.h"
#include "Components/ProgressBar.h"

void UDeflectCooldownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//if (CooldownBar)
	//{
	//	CooldownBar->BarFillType = EProgressBarFillType::BottomToTop;
	//}
}

void UDeflectCooldownWidget::UpdateCooldown(float Remaining, float Total)
{
	if (!CooldownBar) return;

	const bool bOnCooldown = Remaining > 0.f;
	CooldownBar->SetVisibility(bOnCooldown ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	if (bOnCooldown)
	{
		const float Percent = (Total > 0.f) ? 1.f - (Remaining / Total) : 1.f;
		CooldownBar->SetPercent(Percent);
	}
}
