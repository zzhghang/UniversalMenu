// HangHang All Rights Reserved


#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"

#include "LocalizationKeys.h"
#include "GenericPlatform/GenericPlatformMisc.h"

void UListDataObject_Editable::SetText(const FText& InText)
{
	FString Filtered = InText.ToString();

	if (CharFilter != EEditableCharFilter::All)
	{
		Filtered.GetCharArray().RemoveAll([this](TCHAR C) -> bool
		{
			const bool bAlpha = FChar::IsAlpha(C);
			const bool bDigit = FChar::IsDigit(C);
			switch (CharFilter)
			{
			case EEditableCharFilter::LettersOnly:  return !bAlpha;
			case EEditableCharFilter::NumbersOnly:  return !bDigit;
			case EEditableCharFilter::Alphanumeric: return !bAlpha && !bDigit;
			default: return false;
			}
		});
	}

	const FString Truncated = Filtered.Left(MaxCharCount);

	if (CurrentText.ToString().Equals(Truncated))
	{
		return;
	}

	CurrentText = FText::FromString(Truncated);
	NotifyListDataModified(this);
}

FString UListDataObject_Editable::GetHashedPassword() const
{
	const FString PlainText = CurrentText.ToString();
	if (PlainText.IsEmpty()) return FString();

	FTCHARToUTF8 Utf8Str(*PlainText);
	FSHA256Signature Hash;
	FPlatformMisc::GetSHA256Signature(reinterpret_cast<const uint8*>(Utf8Str.Get()), Utf8Str.Length(), Hash);
	return Hash.ToString();
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
