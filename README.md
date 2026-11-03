# Real-Time Camera Filter System on Zynq-7000

## Project Overview

This project aims to build a real-time camera filter system for a Zynq-7000-series platform. It will offer **XXX** image filters and use the FPGA board's buttons to let the user switch between them.

The target Zynq device has not been selected yet and is listed as **XXX** for now. The exact board, camera interface, and implementation details will be defined later.

## How It Is Intended to Work

The system is designed to prepare all three filtered versions of each captured frame in parallel. When the user presses a button on the FPGA board, the display should switch to the corresponding filter without waiting for that filter to be computed.

The planned processing flow is:

1. Capture a frame from the camera.
2. Make the frame available to three concurrent filter-processing tasks.
3. Generate one filtered version for each of the **XXX** filter options.
4. Read the user's filter selection from the FPGA board's buttons.
5. Display the most recently available frame using the selected filter.

Preparing the filter outputs in parallel is intended to make filter selection feel immediate. Actual response time will depend on the final hardware, software, and display pipeline.

The implementation will need to coordinate captured frames, filter results, and button events so the display uses a consistent, recent frame.

## Hardware

| Component | Status |
| --- | --- |
| FPGA / SoC | Zynq-7000 series |
| Camera | To be determined |
| Filter selection | FPGA board buttons |
| Display or video output | HDMI |

## Filter Options

The system will provide selectable filters. Their specific effects have not been defined yet:

1. Filter 1 — GrayScale
2. Filter 2 — Blur
3. Filter 3 — Stretch
4. Filter 4 — Contrast

## Project Status

This repository currently contains the project overview. The target board, camera, filter algorithms, software and FPGA partitioning, and build instructions are still to be determined.

## Future Work

- Select the exact Zynq-7000 device and development board.
- Choose and integrate a compatible camera and video output.
- Define the filter algorithms.
- Decide which tasks will run in software and which, if any, will be implemented in programmable logic.
- Map the FPGA buttons to the filter selections.
