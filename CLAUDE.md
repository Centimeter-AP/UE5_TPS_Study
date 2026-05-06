# CLAUDE.md

이 파일은 Claude Code (claude.ai/code)가 이 저장소에서 작업할 때 참고하는 가이드입니다.

## 프로젝트 개요

Unreal Engine 5.7 C++ 프로젝트(`TPSProject`). Variant_Combat 단일 장르(근접 전투)로 운영되며, 그리드 기반 인벤토리 시스템을 개발 중입니다.

- **GitHub**: https://github.com/Centimeter-AP/UE5_TPS_Study (private)
- **엔진 버전**: UE 5.7

## 빌드 방법

Visual Studio 2022에서 `TPSProject.sln`을 열어 빌드하거나, 언리얼 에디터 내 Tools -> Compile로 빌드합니다.

- **빌드 타겟**: `TPSProject` (런타임) 또는 `TPSProjectEditor` (에디터)
- 언리얼 에디터가 열려있으면 빌드 실패하니 반드시 에디터 닫고 실행
- 빌드 성공 시 출력 마지막에 `Result: Succeeded` 확인
- 에러 발생 시 자동으로 수정 후 재빌드
- **파일 추가 시마다 빌드로 확인**

프로젝트 파일 재생성: `TPSProject.uproject` 우클릭 -> "Generate Visual Studio project files"

## 빌드 명령어

경로에 공백이 포함되어 있어 반드시 PowerShell로 실행:

```powershell
& "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" TPSProjectEditor Win64 Development "C:\Users\CMAP\Documents\Unreal Projects\IDU\TPSProject\TPSProject.uproject"
```

## 아키텍처

### 모듈 구조

단일 런타임 모듈 `TPSProject` (Source/TPSProject/). `TPSProject.Build.cs`에 인클루드 경로 등록.

**모듈 의존성**: `EnhancedInput`, `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `UMG`, `Slate`

### 기본 클래스 (Source/TPSProject/)

- `ATPSProjectCharacter` - 추상 기반 클래스. 스프링암 카메라, Enhanced Input 바인딩(Move/Look/Jump), Blueprint/UI에서 직접 호출 가능한 가상 `Do*` 메서드.
- `ATPSProjectGameMode`, `ATPSProjectPlayerController` - 최소 기반 클래스.

### Variant_Combat (Source/TPSProject/Variant_Combat/)

콤보/차지 공격, 피해/사망/리스폰, AI 적을 갖춘 근접 전투 게임.

- `ACombatCharacter` - `ICombatAttacker` + `ICombatDamageable` 인터페이스 구현. 콤보 스트링(몽타주 섹션 + `AnimNotify_CheckCombo`), 꾹 누르기 차지 공격(`AnimNotify_CheckChargedAttack`), 구체 트레이스 피격 판정(`AnimNotify_DoAttackTrace`), 피격 시 래그돌, 타이머 기반 리스폰 처리.
- `ACombatEnemy` - 동일한 두 인터페이스를 구현하는 AI 캐릭터. StateTree 태스크가 소비하는 `FOnEnemyAttackCompleted`, `FOnEnemyLanded` 델리게이트를 노출.
- `ACombatAIController` - `StateTree` 에셋을 실행. 커스텀 StateTree 태스크/조건은 `CombatStateTreeUtility.h`에 정의.
- `ACombatEnemySpawner` - 적 스폰 담당; `ACombatActivationVolume`이 스포너를 트리거.
- EQS 컨텍스트: `EnvQueryContext_Player` / `EnvQueryContext_Danger`가 AI의 Environment Query 쿼리에 사용됨.
- 인터페이스: `ICombatAttacker` (DoAttackTrace / CheckCombo / CheckChargedAttack), `ICombatDamageable` (ApplyDamage / HandleDeath / ApplyHealing / NotifyDanger).

### Inventory (Source/TPSProject/Variant_Combat/Inventory/) - 개발 중

디아블로2 / RE4 스타일 그리드 기반 인벤토리 시스템. 탭 구조로 기본 인벤토리 + 가방 탭을 관리한다.

#### 데이터 정의 (`ItemData.h`)

- `EItemType` - `Consumable`(소모품) / `Material`(재료) / `Equipment`(장착) / `Bag`(가방) / `Quest`(퀘스트)
- `FItemData` (USTRUCT) - 아이템 데이터: ID, 이름, 설명, 타입, 아이콘, `GridSize`, `MaxStackSize`, `bIsDroppable`
- `UItemDataAsset` (UPrimaryDataAsset) - 에디터에서 아이템을 개별 `.uasset`으로 생성. 우클릭 -> Miscellaneous -> Data Asset -> UItemDataAsset

#### 런타임 인벤토리 (`InventoryComponent.h/.cpp`)

- `FInventoryItem` (USTRUCT) - 인스턴스: `DataAsset` 참조, `StackCount`, `Position`(좌상단 그리드 좌표), `bIsRotated`, `ItemGuid`(인스턴스 고유 ID)
- `FInventoryTab` (USTRUCT) - 탭 하나: `TabName`, `TabIcon`, `SourceBagGuid`(가방 탭이면 해당 아이템 Guid), `Items` 배열
- `UInventoryComponent` (UActorComponent) - 전체 인벤토리 관리
  - `GridWidth` / `GridHeight`: 모든 탭이 공유하는 그리드 크기
  - `TryAddItem()`: 스택 합산 우선, 공간 부족 시 새 슬롯 배치
  - `RemoveItem()`: Guid로 수량 제거
  - `MoveItem()`: 탭 간 이동 포함, AABB 충돌 검사
  - `ExpandGrid()`: 그리드 확장 (축소 불가), 모든 탭에 즉시 반영
  - `AddBagTab()` / `RemoveBagTab()`: 가방 획득/드롭 시 탭 추가/제거
  - 델리게이트: `OnInventoryChanged`, `OnTabAdded`, `OnTabRemoved`, `OnGridResized`

**인벤토리 설계 원칙**:
- 모든 탭은 동일한 `GridWidth x GridHeight`를 공유. 그리드 확장 시 전체 탭에 반영
- 가방(`EItemType::Bag`) 획득 → `AddBagTab()` 호출 → UI에 새 탭 추가. 드롭 시 `RemoveBagTab()`으로 제거 (탭 내 아이템 소멸)
- 스택은 `GridSize == (1,1)` && `MaxStackSize > 1`인 아이템만 허용
- 아이템 회전(90도): `bIsRotated == true`이면 `GridSize`를 `(Y, X)`로 해석
- `Equipment`와 `Bag`은 `MaxStackSize = 1` (스택 불가)

### 주요 패턴

- **입력 라우팅**: 모든 캐릭터 클래스는 `UInputAction` 바인딩과 함께 `virtual void Do*(...)` BlueprintCallable 메서드를 제공합니다. 게임패드/키보드와 온스크린 UI 버튼이 동일한 코드 경로를 공유합니다.
- **애니메이션 기반 게임플레이**: 공격 피격 판정, 콤보 연결, 대시 종료는 모두 타이머가 아닌 `AnimNotify` 클래스로 트리거됩니다.
- **StateTree AI**: 적 AI는 비헤이비어 트리가 아닌 UE5의 StateTree를 사용합니다. 커스텀 태스크/조건은 `FStateTreeTaskCommonBase` / `FStateTreeConditionCommonBase`를 상속하는 USTRUCT이며 내부에 `FInstanceDataType`을 중첩 정의합니다.
- **추상 기반 클래스**: 캐릭터, 게임 모드, 컨트롤러 모두 `UCLASS(abstract)`입니다. 에디터에서 Blueprint 자식 클래스를 만들어야 인스턴스화할 수 있습니다.