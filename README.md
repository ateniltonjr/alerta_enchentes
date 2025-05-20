![Capa do Projeto](https://github.com/user-attachments/assets/f2a5c9b8-6208-4723-8f46-1d74be421827)
# Estação de Alerta de Enchentes com RP2040

### **Autor:** Atenilton S. Souza Júnior

**GitHub:** ateniltonjr

## 📌 Sumário
- [Funcionalidades](#-funcionalidades)
- [Hardware Necessário](#-hardware-necessário)
- [Compilação e Upload](#-compilação-e-upload)
- [Vídeo Demonstrativo](#-vídeo-demonstrativo)

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

### ⚙️ Compilação e Upload
### Pré-requisitos
- [SDK do Raspberry Pi Pico](https://github.com/raspberrypi/pico-sdk)
  
- [Toolchain ARM GCC](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
  
- CMake ≥ 3.12
  
- FreeRTOS-Kernel

### Passo a Passo

bash

### Clone o repositório

```git clone https://github.com/ateniltonjr/alerta_enchentes.git```

- Importe o projeto com a extensão do raspberry pi pico

- Caso não tenha baixado FreeRTOS, faça o download do zip e extraia por meio do link: [FreeRTOS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel.git)

- No CmakeLists.txt, susbtitua o endereço do FreeRTOSS-Kernel pelo local em que esteja do seu computador.

Substitua isso:

```set(FREERTOS_KERNEL_PATH "C:/Users/ateni/CODIGOS/TESTES/FreeRTOS-Kernel/")```

por isso:

```set(FREERTOS_KERNEL_PATH "C:/caminho/correto/para/FreeRTOS-Kernel")```

- Apos esta etapa, na extensão do raspberry pi pico, clique na opção ```Clean Cmake``` para atualizar a build.

- Feito esta etapa: conecte a placa ao computador, compile e envie para a placa.

## 🎥 Vídeo Demonstrativo
Assista no YouTube
[Vídeo](https://www.youtube.com/watch?v=K96oKxeMri0)
