// TPTGameplayTagList.h


#if defined(TAG) && defined(CUETAG)
#error "TAG와 CUETAG는 동시에 정의될 수 없습니다."
#elif defined(TAG)
#include "Tag_Character.inl"
#include "Tag_Data.inl"
#include "Tag_Event.inl"
#include "Tag_InputTag.inl"
#include "Tag_Objects.inl"
#elif defined(CUETAG)
#include "CueTag_Effect.inl"
#include "CueTag_Sound.inl"
#include "CueTag_Notify.inl"
#endif