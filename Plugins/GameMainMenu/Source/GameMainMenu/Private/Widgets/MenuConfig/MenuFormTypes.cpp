#include "Widgets/MenuConfig/MenuFormTypes.h"
#include "Misc/DefaultValueHelper.h"

bool FMenuEntryCondition::Matches(const FString& Actual) const
{
	switch (Comparison)
	{
	case EMenuComparison::Equals: return Actual == Value;
	case EMenuComparison::NotEquals: return Actual != Value;
	case EMenuComparison::IsTrue: return Actual.Equals(TEXT("true"), ESearchCase::IgnoreCase);
	case EMenuComparison::IsFalse: return Actual.Equals(TEXT("false"), ESearchCase::IgnoreCase);
	case EMenuComparison::IsEmpty: return Actual.IsEmpty();
	case EMenuComparison::IsNotEmpty: return !Actual.IsEmpty();
	default: break;
	}
	double Left = 0, Right = 0;
	if (!FDefaultValueHelper::ParseDouble(Actual, Left) || !FMath::IsFinite(Left) ||
		!FDefaultValueHelper::ParseDouble(Value, Right) || !FMath::IsFinite(Right)) return false;
	switch (Comparison)
	{
	case EMenuComparison::Greater: return Left > Right;
	case EMenuComparison::GreaterOrEqual: return Left >= Right;
	case EMenuComparison::Less: return Left < Right;
	case EMenuComparison::LessOrEqual: return Left <= Right;
	default: return false;
	}
}
