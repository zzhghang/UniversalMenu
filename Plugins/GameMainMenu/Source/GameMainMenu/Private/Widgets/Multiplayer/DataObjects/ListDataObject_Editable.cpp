// HangHang All Rights Reserved


#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"

#include "LocalizationKeys.h"

void UListDataObject_Editable::SetText(const FText& InText)
{
	const FString Raw = InText.ToString();
	FString Filtered;
	Filtered.Reserve(Raw.Len() + 1);

	if (CharFilter == EEditableCharFilter::All)
	{
		Filtered = Raw;
	}
	else
	{
		for (const TCHAR C : Raw)
		{
			const bool bAlpha = FChar::IsAlpha(C);
			const bool bDigit = FChar::IsDigit(C);
			bool bKeep;
			switch (CharFilter)
			{
			case EEditableCharFilter::LettersOnly:  bKeep = bAlpha;          break;
			case EEditableCharFilter::NumbersOnly:  bKeep = bDigit;          break;
			case EEditableCharFilter::Alphanumeric: bKeep = bAlpha || bDigit; break;
			default: bKeep = true; break;
			}
			if (bKeep) Filtered.AppendChar(C);
		}
	}

	CurrentText = FText::FromString(Filtered.Left(MaxCharCount));
	NotifyListDataModified(this);
}

bool UListDataObject_Editable::ValidateText(const FText& InText, FText& OutReason) const
{
	const FString Text = InText.ToString();

	if (MaxCharCount > 0 && Text.Len() > MaxCharCount)
	{
		OutReason = FText::Format(TXT_MP_VALIDATION_MAX_CHARS(), FText::AsNumber(MaxCharCount));
		return false;
	}

	if (CharFilter == EEditableCharFilter::All)
	{
		return true;
	}

	for (const TCHAR C : Text)
	{
		const bool bAlpha = FChar::IsAlpha(C);
		const bool bDigit = FChar::IsDigit(C);
		bool bValid = true;

		switch (CharFilter)
		{
		case EEditableCharFilter::LettersOnly:  bValid = bAlpha; break;
		case EEditableCharFilter::NumbersOnly:  bValid = bDigit; break;
		case EEditableCharFilter::Alphanumeric: bValid = bAlpha || bDigit; break;
		default: break;
		}

		if (!bValid)
		{
			switch (CharFilter)
			{
			case EEditableCharFilter::LettersOnly:  OutReason = TXT_MP_VALIDATION_LETTERS_ONLY();  break;
			case EEditableCharFilter::NumbersOnly:  OutReason = TXT_MP_VALIDATION_NUMBERS_ONLY();  break;
			case EEditableCharFilter::Alphanumeric: OutReason = TXT_MP_VALIDATION_ALPHANUMERIC();  break;
			default: break;
			}
			return false;
		}
	}

	return true;
}

void UListDataObject_Editable::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	if (MaxCharCount <= 0)
	{
		MaxCharCount = bIsPassword ? 6 : 16;
	}
}
