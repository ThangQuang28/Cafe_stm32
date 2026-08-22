# Cafe_stm32

Firmware điều khiển máy pha cà phê chạy trên vi điều khiển **STM32F030C8Tx**. Dự án sử dụng STM32 HAL, FreeRTOS và các middleware riêng cho cảm biến nhiệt độ, gia nhiệt, bơm, nút nhấn, phát hiện zero-crossing và phát hiện mất AC.

Các thông số nhiệt độ, PID và thời gian pha hiện vẫn là giá trị trên lý thuyết cần được hiệu chỉnh trước khi vận hành máy.

## Tính năng hiện tại

- Đọc hai kênh NTC qua ADC và DMA.
- Điều khiển heater và pump bằng GPIO.
- Điều khiển nhiệt độ boiler bằng PID.
- Điều chế heater dạng time-proportioning với cửa sổ 2 giây.
- Đọc nút `BTN_UP` để bắt đầu chu trình pha.
- Đọc nút `BTN_DOWN` để kết thúc chu trình pha hoặc xác nhận lỗi bằng nhấn giữ.
- Theo dõi zero-crossing và trạng thái nguồn AC.
- Đưa hệ thống về trạng thái an toàn khi lỗi cảm biến hoặc mất AC trong lúc đang chạy.
- Ghi log qua module trace khi cấu hình debug được bật.

## Phần cứng và cấu hình

| Hạng mục      | Cấu hình                              |
| ---           | ---                                   |
| MCU           | STM32F030C8Tx, package LQFP48         |
| Debug/program | ST-LINK qua SWD                       |
| Clock         | HSE ngoài, PLL x4 theo cấu hình CubeMX|
| RTOS          | FreeRTOS kernel V10.0.1, tick 1 kHz   |
| ADC           | ADC_IN0, DMA1 Channel 1               |
| UART          | USART1 trên PA9/PA10                  |
| Heater        | PB14, nhãn `HEAT`                     |
| Pump          | PB13, nhãn `PUMP`                     |
| SWD           | PA13/PA14                             |

Cấu hình chân và peripheral nằm trong [`zenFWCoreOS.ioc`](zenFWCoreOS.ioc) sử dụng SMT32MX để mở xem cấu hình

## Luồng hoạt động

Task ứng dụng trong [`app/zapp/zapp.c`](app/zapp/zapp.c) cập nhật input mỗi 500 ms và chạy máy theo các trạng thái:

```text
IDLE
  | BTN_UP + cảm biến hợp lệ
  v
HEATING -- đạt nhiệt độ mục tiêu --> BREWING -- hết thời gian / BTN_DOWN --> DONE
   |                                      |
   +-- quá thời gian --------------------+
   |                                      |
   +----------- lỗi cảm biến hoặc AC ----> FAULT

FAULT -- nhấn giữ BTN_DOWN + cảm biến hợp lệ --> IDLE
DONE  ---------------------------------------> IDLE
```

Các giá trị đang dùng:

- Nhiệt độ boiler mục tiêu: `90.0 °C`.
- Timeout gia nhiệt: `120 giây`.
- Thời gian bơm trong một shot: `25 giây`.
- Chu kỳ task và sample time PID: `500 ms`.
- Cửa sổ điều chế heater: `4 x 500 ms = 2 giây`.

Các giá trị này được định nghĩa hiện tại trên lý thuyết

## Cấu trúc thư mục

```text
app/       Điểm vào ứng dụng và task điều khiển máy pha
Core/      Mã khởi tạo STM32CubeMX, HAL và FreeRTOS
mid/       Middleware: NTC, PID, heater, pump, button, buzzer, AC và zero-cross
driver/    CMSIS, HAL và BSP/driver nền tảng
Drivers/   STM32 HAL/CMSIS được cung cấp kèm dự án
build/     Makefile và các file build sinh tự động
*.ioc      Cấu hình STM32CubeMX
*.ld       Linker script cho STM32F030C8Tx
```

## Hiệu chỉnh trước khi chạy thật

1. Kiểm tra chiều và giới hạn nhiệt độ của cả hai kênh NTC.
2. Hiệu chỉnh `BOILER_TARGET_TEMP_X10`, `BOILER_HEATING_TIMEOUT_MS` và `BREW_DURATION_MS` theo máy thật.
3. Hiệu chỉnh các hệ số `pid_kp`, `pid_ti`, `pid_td` trong `app/zapp/zapp.c`.
4. Kiểm tra relay/triac heater ở trạng thái mặc định là OFF khi khởi động và khi có lỗi.
5. Kiểm tra tín hiệu AC, zero-crossing và cơ chế dừng pump bằng phần cứng.
6. Chạy thử với tải giới hạn và giám sát nhiệt độ trước khi pha một shot đầy đủ.

## An toàn

Firmware có kiểm tra lỗi NTC và AC trước khi cho phép chu trình chạy, đồng thời tắt heater/pump khi vào `FAULT`. Đây không phải là cơ chế bảo vệ duy nhất: mạch công suất vẫn cần cầu chì, bảo vệ quá nhiệt, chống giật và các liên động phần cứng độc lập.

## Ghi chú phát triển

- Thay đổi chân GPIO hoặc peripheral nên thực hiện trong `zenFWCoreOS.ioc`, sau đó kiểm tra lại mã sinh ra.
- Có 2 chân NTC nhưng hiện tại Hardware mới có 1 chân nên đang comment lại thành 2 chân chung nhau. Vì vậy sau khi phát triển cần kiểm tra mở lại 2 chân NTC dòng `return gh_ntc.val_temp[0]` trong hàm `uint16_t ntc_bsp_get_raw(uint8_t channel)` thuộc file `ntc_bsp.c`
- PWM đã check có hoạt động nhưng chưa confirm 100% nên cần check lại xung có hoạt động đúng hay không
- Điều khiển tốc độ có thể dựa theo các case của PID_OUT và hiện đang chưa có do chưa có sản phẩm để kiểm chứng.
- NTC hoạt động dựa theo môi trường hiện tại cũng chưa confirm 100% cần test lại khi thay đổi môi trường nhanh từ lạnh - nóng ( cho vào cốc đá rồi bỏ ra ngoài )
