# SoC_HW1_C110112124
# FPGA LED 跑馬燈專案_AXI4Lite練習題

## 專案概述

本專案使用 FPGA 實現 LED 跑馬燈效果，並能透過按鈕與開關控制顯示速度與樣式。

## 功能

- **LED 跑馬燈：** LED 按順序依次亮起，呈現跑馬燈效果。
- **速度控制：** 按下按鈕可以加速跑馬燈的效果。
- **反向控制：** 使用開關可以反轉 LED 顯示。

## 硬體

- **開發板：** EGO-XZ7（Zynq-7000 SoC，型號：XC7Z020CLG484-1）
- **處理器：** ARM Cortex-A9
- **LED：** 展示跑馬燈效果
- **按鈕：** 用於控制跑馬燈的速度
- **開關：** 用於反轉 LED 顯示

## 軟體工具

- **Xilinx Vivado 2018：** 用於硬體設計與合成。
- **Xilinx SDK：** 用於開發軟體並控制 PS。
  
## 工作原理
電路中包含PS、AXI匯流排與自己建立的AXI4Lite規格的IP，將PS產生的LED訊號透過AXI傳到IP，在IP內與開關輸入訊號進行邏輯運算處理後再將LED訊號輸出顯示。同時也將IP端輸入的按鈕訊號回傳至PS端，用以改變PS端產生之LED訊號的顯示速度。

- **跑馬燈：**<br>
  PS 根據c程式生成跑馬燈信號，並透過AXI寫入LEDrunning的slv_reg0暫存器。LEDrunning讀取來自slv_reg0的跑馬燈資料，並將處理後的LED資料輸出顯示。
- **按鈕控制：**<br>
  LEDrunning輸入按鈕狀態並寫入slv_reg1中。PS端讀取slv_reg1的資料，用以改變跑馬燈的顯示速度。
- **開關控制：**<br>
  LEDrunning輸入開關狀態並與來自slv_reg0的LED資料做XOR運算處理。
  
## Block 設計
![螢幕擷取畫面 2025-04-26 142902](https://github.com/user-attachments/assets/e375f0a0-019b-4057-a303-77d2e80088cc)



## Demo 影片

以下是展示 FPGA LED 跑馬燈功能的影片：


https://github.com/user-attachments/assets/9a60ccbf-8521-440a-8b47-a3a73cae1e72







