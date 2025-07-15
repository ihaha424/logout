#pragma once

#define ENUM_TAG_GEN									\
		#define TPT_TAG_GEN(Type, Name, Desc)			\
		Tag_##Type##_##Name UMETA(DisplayName = Desc),	\
		#include "./Inline/TPTGameplayTagList.inl"		\
		#undef TPT_TAG_GEN								


#define GAMEPLAY_TAG_GEN						\
	#define TPT_TAG_GEN(Type, Name, Desc) 		\
	FGameplayTag Tag_##Type##_##Name;			\
	#include "./Inline/TPTGameplayTagList.inl"	\
	#undef TPT_TAG_GEN							


#define ADD_NATIVE_GAMEPLAYTAG_GEN																							\
	#define TPT_TAG_GEN(Type, Name, Desc)																					\
	GameplayTags.Tag_##Type##_##Name = Manager.AddNativeGameplayTag(TEXT("TPTGamePlayTag." #Type "." #Name), TEXT(Desc));   \
	if (!GameplayTags.Tag_##Type##_##Name.IsValid())                                                             			\
	{                                                                                                                       \
		GameplayTags.Tag_##Type##_##Name = FGameplayTag::RequestGameplayTag(TEXT("TPTGamePlayTag." #Type "." #Name));       \
	}                                                                                                                       \
	#include "./Inline/TPTGameplayTagList.inl"																				\
	#undef TPT_TAG_GEN																										


#define BIND_TAG_TO_ENUM																				\
	#define TPT_TAG_GEN(Type, Name, Desc)																\
	GameplayTags.TagMap.Add(GameplayTags.Tag_##Type##_##Name, EFTPTGameplayTags::Tag_##Type##_##Name);	\
	#include "./Inline/TPTGameplayTagList.inl"															\
	#undef TPT_TAG_GEN																					






#define EXPORTER_TAG_GEN_HEADER	\
#define TPT_TAG_GEN(Type, Name, Desc) TEXT("GameplayTagList=(Tag=\"TPTGamePlayTag." #Type "." #Name "\", DevComment=\"" Desc "\")\n") \
static const TCHAR* GeneratedTagLines =	\


#define EXPORTER_TAG_GEN_TAIL \
; \
#undef TPT_TAG_GEN \


#define TAG true
#define TPT_TAG_GEN(Type, Properties, Name, Description) TEXT("GameplayTagList=(Tag=\"TPTGamePlayTag." #Type "." #Properties "." #Name "\", DevComment=\"" Desc "\")\n")
static const TCHAR* GeneratedTagLines =
#include "./Inline/TPTGameplayTagList.inl"
;

#undef TPT_TAG_GEN
#undef TAG

#define CUETAG true
#define TPT_TAG_GEN(Type, Name, Desc) TEXT("GameplayTagList=(Tag=\"TPTGamePlayTag." #Type "." #Name "\", DevComment=\"" Desc "\")\n")
static const TCHAR* GeneratedCueTagLines =
#include "./Inline/TPTGameplayTagList.inl"
;

#undef TPT_TAG_GEN
#undef CUETAG

#define TAG true
#define TPT_TAG_GEN(Type, Name, Desc) TEXT("GameplayTagList=(Tag=\"TPTGamePlayTag." #Type "." #Name "\", DevComment=\"" Desc "\")\n")
static const TCHAR* GeneratedTagLines =
#include "./Inline/TPTGameplayTagList.inl"
;

#undef TPT_TAG_GEN
#undef TAG