# 🌊 Estação de Alerta de Enchentes com RP2040

**Projeto desenvolvido por Atenilton Júnior**  

## 📌 Sumário
- [Funcionalidades](#-funcionalidades)
- [Hardware Necessário](#-hardware-necessário)
- [Compilação e Upload](#-compilação-e-upload)
- [Estrutura do Código](#-estrutura-do-código)
- [Vídeo Demonstrativo](#-vídeo-demonstrativo)
- [Licença](#-licença)

---

## 🚀 Funcionalidades
| Módulo | Descrição |
|--------|-----------|
| **Monitoramento** | Leitura analógica contínua do joystick (eixos X/Y) simulando sensores de água e chuva |
| **Alertas** | Ativação de LED vermelho, buzzer e matriz 5x5 quando:<br>• Nível da água ≥ 70%<br>• Volume de chuva ≥ 80% |
| **Display OLED** | Exibe em tempo real:<br>• Valores percentuais<br>• Mensagens de alerta personalizadas |
| **Feedback Sonoro** | Padrões distintos para cada tipo de alerta (água, chuva ou ambos) |

---

## 🔧 Hardware Necessário
| Componente | Descrição |
|------------|-----------|
| **Placa BitDog Lab** | RP2040 com joystick integrado |
| **LEDs** | RGB (GPIO 11, 12, 13) |
| **Buzzer** | Ativo em GPIO definido na lib |
| **Display OLED** | 128x64 pixels (I²C) |
| **Matriz de LEDs** | 5x5 para ícones de alerta |

---

## ⚙️ Compilação e Upload
### Pré-requisitos
- [SDK do Raspberry Pi Pico](https://github.com/raspberrypi/pico-sdk)
- [Toolchain ARM GCC](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- CMake ≥ 3.12

### Passo a Passo
```bash
# Clone o repositório
git clone https://github.com/seuusuario/alerta_enchentes.git
cd alerta_enchentes

# Configure o build
mkdir build && cd build
cmake ..

# Compile
make -j4

# Grave na placa (modo BOOTSEL)
cp alerta_enchentes.uf2 /media/user/RPI-RP2/

## 🏗️ Estrutura do Código
alerta_enchentes/
├── src/
│   ├── main.c              # Inicialização e criação de tarefas
│   ├── tarefas_utilizadas.h # Lógica das tarefas FreeRTOS
│   └── lib/                # Drivers para display, buzzer, etc.
├── CMakeLists.txt
└── README.md

Tarefas FreeRTOS
Tarefa	Função
vJoy_x_task	Leitura do eixo X (nível da água)
vLedGreenTask	Controle PWM do LED verde
vAlertTask	Verificação de condições de alerta
vBuzzerTask	Geração de sinais sonoros
🎥 Vídeo Demonstrativo
Assista no YouTube (Substituir VIDEO_ID pelo link real)