# UE5 TPS Project

Unreal Engine 5 C++ 기반 3인칭 근접 전투 게임 프로젝트입니다.  
전투 시스템, StateTree AI, 그리드 기반 인벤토리 시스템을 엔진 기능에 의존하지 않고 직접 C++로 설계·구현하는 데 중점을 두었습니다.

---

## 기술 스택

| 분류 | 내용 |
|------|------|
| **엔진** | Unreal Engine 5.7 |
| **언어** | C++ |
| **입력** | Enhanced Input System |
| **AI** | StateTree + EQS (Environment Query System) |
| **UI** | UMG (Unreal Motion Graphics) |
| **데이터** | Primary Data Asset |

---

## 구현 기능

### ⚔️ 전투 시스템
- **콤보 공격** — 몽타주 섹션 기반 멀티 히트 콤보 스트링. `AnimNotify`로 타이머 없이 콤보 입력 여부를 판단
- **차지 공격** — 입력 홀드 감지 및 차지 루프 처리. 릴리즈 시점에 공격 판정 실행
- **구체 트레이스 판정** — 공격 타이밍에 맞춰 `AnimNotify`가 Sphere Trace를 발동, 범위 내 적에게 피해 적용
- **피격 반응 / 사망 / 리스폰** — 피격 시 래그돌 전환, 타이머 기반 리스폰 처리

### 🤖 AI 시스템
- **StateTree 기반** — UE5의 StateTree로 적 행동 제어. Behavior Tree 대신 StateTree를 채택해 상태 전이와 조건을 구조적으로 관리
- **EQS** — `EnvQueryContext_Player` / `EnvQueryContext_Danger`로 플레이어 위치 및 위험 지점 쿼리
- **스포너** — `ACombatActivationVolume` 볼륨 진입 시 적 그룹 스폰

### 🎒 인벤토리 시스템
- **그리드 배치** — 디아블로2 스타일 격자 인벤토리. 아이템 배치 시 AABB 충돌 검사로 겹침 방지
- **탭 구조** — 기본 인벤토리와 가방 탭을 동일한 그리드 크기로 관리. 가방 획득 시 탭 동적 추가/제거
- **스택 시스템** — 1×1 아이템에 한해 자동 수량 합산. `MaxStackSize` 초과 시 새 슬롯 생성
- **아이템 회전** — 90도 회전 배치 지원 (`bIsRotated`로 GridSize X·Y 스왑)
- **Data Asset** — `UItemDataAsset` (UPrimaryDataAsset 상속)으로 아이템 데이터를 에디터에서 독립 에셋으로 관리
- **아이템 타입** — Consumable / Material / Equipment / Bag / Quest

### 🖥️ UI
- **인벤토리 토글** — I키로 열기/닫기. BeginPlay에서 1회 생성 후 Visibility로 표시 전환 (상태 유지)
- **입력 모드 전환** — 인벤토리 열릴 때 Game+UI 모드로 전환, 닫힐 때 Game 모드 복귀

### 🕹️ 플레이어 인터랙션
- **아이템 픽업** — SphereComponent Overlap으로 근접 감지, F키 입력 시 인벤토리에 추가. 수량 소진 시 액터 자동 제거
- **Enhanced Input** — 키보드/게임패드가 동일한 코드 경로(`Do*` 메서드)를 공유

---

## 아키텍처

```
TPSProject (단일 런타임 모듈)
└── Variant_Combat/
    ├── CombatCharacter       기반 캐릭터 — 전투, 인벤토리, 입력 통합
    ├── AI/                   StateTree 태스크·조건, EQS 컨텍스트, 스포너
    ├── Animation/            AnimNotify 클래스 (판정·콤보·차지)
    ├── Gameplay/             ItemPickup, ActivationVolume 등 월드 오브젝트
    ├── Interfaces/           ICombatAttacker, ICombatDamageable
    ├── Inventory/            UInventoryComponent, FItemData, UItemDataAsset
    └── UI/                   UInventoryWidget, UCombatLifeBar
```
