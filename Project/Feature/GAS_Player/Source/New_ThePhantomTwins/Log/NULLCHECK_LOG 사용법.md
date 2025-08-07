# NULLCHECK 매크로 사용 가이드

C++ 코드 내에서 포인터의 `nullptr` 여부를 검사할 때 사용할 수 있는 매크로들입니다.
다음 매크로들은 코드 가독성과 안전성을 높이는 데 도움이 됩니다.

---

## 1. `NULLCHECK_LOG`

**✔ 설명**

포인터가 `nullptr`인 경우 로그를 출력합니다.

**주의:** *명시적으로 `}`를 닫아주어야 합니다.*

**사용 예시**

```cpp
NULLCHECK_LOG(Target, CategoryName, Verbosity)
}

```

**확장 형태**

```cpp
if (nullptr == Target)
{
    TPT_LOG(CategoryName, Verbosity, TEXT(#Target " is nullptr."));
    // '}' 없음! 직접 닫아줘야 함
}

```

**비고**

- 이후 코드를 작성하고 직접 `}`를 닫아야 합니다.

---

## 2. `NULLCHECK_CODE_LOG`

**✔ 설명**

`nullptr` 체크 후 간단한 코드를 실행하고 로그를 출력할 수 있습니다.

**주의:** *역시 `}`를 명시적으로 닫아야 합니다.*

**사용 예시**

```cpp
NULLCHECK_CODE_LOG(Target, CategoryName, Verbosity, CODE)
}

```

**확장 형태**

```cpp
if (nullptr == Target)
{
    TPT_LOG(CategoryName, Verbosity, TEXT(#Target " is nullptr."));
    CODE
    // '}' 없음! 직접 닫아줘야 함
}

```

**비고**

- 내부에 한 줄 이상의 코드가 필요한 경우 사용
- 이후 `}`는 직접 닫아야 함

---

## 3. `NULLCHECK_RETURN_LOG` ✅ **(사용 추천)**

**✔ 설명**

간단하게 `nullptr` 체크 후 로그 출력 후 바로 리턴하고 싶을 때 사용합니다.

**사용 예시**

```cpp
NULLCHECK_RETURN_LOG(Target, CategoryName, Verbosity, ReturnValue)

```

**확장 형태**

```cpp
if (nullptr == Target)
{
    TPT_LOG(CatecoryName, Verbosity, TEXT(#Target "is nullptr."));
    return ReturnValue;
}

```

**비고**

- 한 줄로 안전하게 사용할 수 있음

---

## 4. `NULLCHECK_CODE_RETURN_LOG` ✅ **(사용 추천)**

**✔ 설명**

`nullptr` 체크 후 로그 출력과 간단한 코드 실행과 함께 리턴값을 주고 싶을 때 사용합니다.

**사용 예시**

```cpp
NULLCHECK_CODE_RETURN_LOG(Target, CategoryName, Verbosity, CODE, ReturnValue)

```

**확장 형태**

```cpp
if (nullptr == Target)
{
    TPT_LOG(CatecoryName, Verbosity, TEXT(#Target "is nullptr."));
    CODE
    return ReturnValue;
}

```

**비고**

- 에러 처리, 상태 기록 등 간단한 로직 처리에 적합

---

## ✅ 사용 권장 순위

1. `NULLCHECK_RETURN_LOG`, `NULLCHECK_CODE_RETURN_LOG` (상황에 따라 둘중 하나 선택 권장)
2. `NULLCHECK_LOG` (로그 출력후 많은 로직이 필요시, 명시적으로 닫아줄 때 권장)
3. `NULLCHECK_CODE_LOG` (사용 권장 X)