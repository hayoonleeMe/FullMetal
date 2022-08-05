#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT(TEXT("DataTable'/Game/Data/ClassTable.ClassTable'"));
	if (DT.Succeeded())
	{
		MyStats = DT.Object;
	}
}

FClassData* UMyGameInstance::GetStatData(int32 Class)
{
	return MyStats->FindRow<FClassData>(*FString::FromInt(Class), TEXT(""));
}
