# CLAUDE.md

이 파일은 Claude Code (claude.ai/code)가 이 저장소에서 작업할 때 참고하는 가이드입니다.

## 프로젝트 개요

Unreal Engine 5.7 C++ 프로젝트(`TPSProject`)로, 세 가지 독립적인 게임플레이 변형(Variant)을 포함한 3인칭 캐릭터 템플릿입니다. 각 Variant는 서로 런타임 코드를 공유하지 않는 독립적인 장르 구현체입니다.

## 빌드 방법

Visual Studio 2022에서 `TPSProject.sln`을 열어 빌드하거나, 언리얼 에디터 내 Tools -> Compile로 빌드합니다. 별도의 빌드 스크립트는 없습니다.

- **빌드 타겟**: `TPSProject` (런타임) 또는 `TPSProjectEditor` (에디터)
- **엔진 버전**: UE 5.7 (`TPSProject.uproject`에 명시)

프로젝트 파일 재생성: `TPSProject.uproject` 우클릭 -> "Generate Visual Studio project files"

## 아키텍처

### 모듈 구조

단일 런타임 모듈 `TPSProject` (Source/TPSProject/)로 구성됩니다. 세 Variant 모두 이 모듈 안에 있으며 `TPSProject.Build.cs`에 인클루드 경로가 등록되어 있습니다.

**모듈 의존성**: `EnhancedInput`, `AIModule`, `StateTreeModule`, `GameplayStateTreeModule`, `UMG`, `Slate`

### 기본 클래스 (Source/TPSProject/)

- `ATPSProjectCharacter` - 추상 기반 클래스. 스프링암 카메라, Enhanced Input 바인딩(Move/Look/Jump), 그리고 Blueprint/UI에서 직접 호출 가능한 가상 `Do*` 메서드를 포함.
- `ATPSProjectGameMode`, `ATPSProjectPlayerController` - 최소 기반 클래스.

### Variant_Combat (Source/TPSProject/Variant_Combat/)

콤보/차지 공격, 피해/사망/리스폰, AI 적을 갖춘 근접 전투 게임.

- `ACombatCharacter` - `ICombatAttacker` + `ICombatDamageable` 인터페이스 구현. 콤보 스트링(몽타주 섹션 + `AnimNotify_CheckCombo`), 꾹 누르기 차지 공격(`AnimNotify_CheckChargedAttack`), 구체 트레이스 피격 판정(`AnimNotify_DoAttackTrace`), 피격 시 래그돌, 타이머 기반 리스폰 처리.
- `ACombatEnemy` - 동일한 두 인터페이스를 구현하는 AI 캐릭터. StateTree 태스크가 소비하는 `FOnEnemyAttackCompleted`, `FOnEnemyLanded` 델리게이트를 노출.
- `ACombatAIController` - `StateTree` 에셋을 실행. 커스텀 StateTree 태스크/조건은 `CombatStateTreeUtility.h`에 정의.
- `ACombatEnemySpawner` - 적 스폰 담당; `ACombatActivationVolume`이 스포너를 트리거.
- EQS 컨텍스트: `EnvQueryContext_Player` / `EnvQueryContext_Danger`가 AI의 Environment Query 쿼리에 사용됨.
- 인터페이스: `ICombatAttacker` (DoAttackTrace / CheckCombo / CheckChargedAttack), `ICombatDamageable` (ApplyDamage / HandleDeath / ApplyHealing / NotifyDanger).

### Variant_Platforming (Source/TPSProject/Variant_Platforming/)

고급 이동 기능을 갖춘 3D 플랫포머.

- `APlatformingCharacter` - 더블 점프, 벽 점프(구체 트레이스), 대시(몽타주 기반, `AnimNotify_EndDash`), 코요테 타임. 이동 상태는 `uint8` 비트 플래그로 압축. UI/컨트롤러 입력 공용 처리를 위한 가상 `Do*` 메서드.

### Variant_SideScrolling (Source/TPSProject/Variant_SideScrolling/)

2D 횡스크롤 게임.

- `ASideScrollingCharacter` - 단일 수평축 이동, 더블 점프, 벽 점프, 소프트 플랫폼(콜리전 채널 토글로 아래 통과), 상호작용 반경 구체 검사.
- `ASideScrollingCameraManager` - 횡스크롤 시점 고정을 위한 커스텀 플레이어 카메라 매니저.
- `ASideScrollingGameMode` - HUD 위젯(`USideScrollingUI`) 스폰, `ProcessPickup()`으로 수집 아이템 수 관리.
- AI: `ASideScrollingNPC` + `ASideScrollingAIController` (StateTree 사용, 유틸리티는 `SideScrollingStateTreeUtility.h`).
- 게임플레이 액터: `SideScrollingJumpPad`, `SideScrollingMovingPlatform`, `SideScrollingPickup`, `SideScrollingSoftPlatform`.
- 인터페이스: `ISideScrollingInteractable` (단일 메서드 `Interaction(AActor*)`).

### 주요 패턴

- **입력 라우팅**: 모든 캐릭터 클래스는 `UInputAction` 바인딩과 함께 `virtual void Do*(...)` BlueprintCallable 메서드를 제공합니다. 게임패드/키보드와 온스크린 UI 버튼이 동일한 코드 경로를 공유합니다.
- **애니메이션 기반 게임플레이**: 공격 피격 판정, 콤보 연결, 대시 종료는 모두 타이머가 아닌 `AnimNotify` 클래스로 트리거됩니다.
- **StateTree AI**: 적 AI는 비헤이비어 트리가 아닌 UE5의 StateTree를 사용합니다. 커스텀 태스크/조건은 `FStateTreeTaskCommonBase` / `FStateTreeConditionCommonBase`를 상속하는 USTRUCT이며 내부에 `FInstanceDataType`을 중첩 정의합니다.
- **추상 기반 클래스**: 세 Variant의 캐릭터, 게임 모드, 컨트롤러 모두 `UCLASS(abstract)`입니다. 에디터에서 Blueprint 자식 클래스를 만들어야 인스턴스화할 수 있습니다.

## 빌드 명령어

코드 수정 후 빌드 확인할 때 아래 명령어 사용:

```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" TPSProjectEditor Win64 Development "C:\Users\CMAP\Documents\Unreal Projects\IDU\TPSProject\TPSProject.uproject"
```

- 언리얼 에디터가 열려있으면 빌드 실패하니 반드시 에디터 닫고 실행
- 빌드 성공 시 `Build: 1 succeeded` 메시지 확인
- 에러 발생 시 자동으로 수정 후 재빌드
