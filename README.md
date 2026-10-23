# LowEngine

SDL3 창에서 Wavefront OBJ 메시를 표시하는 C++23 / Vulkan 1.3 렌더러입니다.
vk-bootstrap으로 인스턴스, 물리 장치, 논리 장치, 스왑체인을 구성합니다.
Vulkan 1.3의 dynamic rendering과 synchronization2를 사용합니다.

## 빌드 및 실행

필수 도구: CMake 3.24 이상, C++23 컴파일러, Vulkan 1.3 개발 패키지와
`glslc`, SDL3 개발 패키지, GLM. 실행 환경에는 Vulkan 1.3의
`dynamicRendering`과 `synchronization2`를 지원하는 GPU/드라이버가 필요합니다.

```sh
git submodule update --init --recursive
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/LowEngineBuild
./build/LowEngineBuild /path/to/Model.obj
./build/LowEngineBuild /path/to/Model.obj --validation --frames 120
```

`--validation`은 설치된 `VK_LAYER_KHRONOS_validation`을 활성화합니다.
`--frames N`은 N프레임 제출 후 자동 종료하며, 생략하면 창을 닫을 때까지 실행합니다.
`--help`로 사용법을 확인할 수 있습니다. Visual Studio 등 다중 구성 빌드는
`--config Debug`를 추가하며 실행 파일은 `build/Debug`에 생성됩니다.

셰이더는 빌드 중 SPIR-V로 컴파일됩니다. `Shaders`와 `Assets`는 실행 파일 옆에
배치되므로 다른 작업 디렉토리에서도 실행할 수 있습니다. 배포 시 이 두 폴더를
실행 파일과 함께 복사해야 합니다. 모델 경로를 생략하면 기본 큐브를 표시합니다.

## 기능과 조작

- OBJ `v`, `vt`, `vn`, `f`, 양수/음수 인덱스와 네 가지 face 표기 지원.
- 평면의 단순 오목/볼록 다각형 삼각분할. 노멀이 없으면 면 노멀 생성.
- 모델을 원점 중심의 단위 구 안에 맞추고 방향광과 깊이 테스트로 렌더링.
- 왼쪽 마우스 드래그: 궤도 회전. 휠: 확대/축소.
- `Space`: 자동 회전 전환. `R`: 시점 초기화. `Esc`: 종료.
- 창 크기·픽셀 밀도 변경 시 스왑체인, 깊이 이미지, 파이프라인 재생성.
- 최소화 중 렌더링 대기. 초기화 실패를 포함한 자원 정리 및 오류 출력.

현재는 단색 메시 렌더러입니다. UV 인덱스는 검증하지만 텍스처·MTL 재질은
표시하지 않으며, `s` smoothing group 없이 누락된 노멀은 면 단위로 생성합니다.
비평면/자기 교차 다각형, 선·점 프리미티브, vertex-color 확장, OBJ line continuation은
지원하지 않습니다. 성능보다 구조의 단순함을 우선하여 프레임 하나를 처리하고,
메시 버퍼에는 host-visible coherent 메모리를 사용합니다.

## 구조와 코드 규칙

- `Include/GraphicEngine/Mesh`, `Src/GraphicEngine/Mesh`: OBJ 로딩 및 메시.
- `Include/GraphicEngine/Vulkan`, `Src/GraphicEngine/Vulkan`: Vulkan 렌더러와 SDL3 창.
- `Src/Main.cxx`: 명령행 옵션과 실행 진입점.
- `Shaders`, `Assets`, `Tests`: 셰이더, 기본 OBJ, 로더 회귀 테스트.

프로젝트 코드는 `#include <...>`만 사용합니다. 디렉토리와 클래스·함수·공개 멤버는
PascalCase, 파라미터는 camelCase, private 멤버 변수는 `_camelCase`입니다.
헤더는 `.h`, C++ 소스는 `.cxx`이며 `UI` 디렉토리는 사용하지 않습니다.
C++/GLSL 필수 진입점 `main`과 외부 API 식별자는 원래 표기를 따릅니다.
기존 `external/vk-bootstrap` 서브모듈은 업스트림 경로와 내부 코드를 유지합니다.

SDL3 Vulkan 연동은 [SDL 공식 문서](https://wiki.libsdl.org/SDL3/CategoryVulkan)를,
이미지별 present 세마포어 관리는
[Khronos 동기화 지침](https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html)을 참고했습니다.
