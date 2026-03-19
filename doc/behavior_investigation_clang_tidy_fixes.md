# Behavior Investigation: Clang-Tidy / Pre-commit Fixes

This document summarizes whether the code changes made for clang-tidy and pre-commit could alter runtime behavior. **Conclusion: no intentional behavior change; one pre-existing timestamp semantics issue is documented below.**

## 1. Changes with no runtime effect

| Change                                                                                              | Reason                                                                                                                |
| --------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------- |
| Header guards, `#pragma once` → `#ifndef`/`#define`/`#endif`                                        | Compile-time only.                                                                                                    |
| `= delete` for copy/move constructors and assignment                                                | Same semantics as before (no copy/move was used).                                                                     |
| `inline` on function definitions in headers                                                         | ODR-compliant; same generated code.                                                                                   |
| Initialization of variables (`tmp_yaw=0`, `comma='\0'`, `log_level_`, `protocol_`, `token=nullptr`) | All variables are assigned before use; initial values are overwritten. Same behavior.                                 |
| `return (lidar_parameter_set() == 0)` instead of `if (...) return false; return true`               | Logically identical.                                                                                                  |
| `publish(ros_msg)` instead of `publish(std::move(ros_msg))`                                         | `publish` takes `const Message&`; no move. Same.                                                                      |
| `publish(*inno_scan_msg_)` instead of `publish(std::move(inno_scan_msg_))`                          | Message content is the same; only the way it is passed (by reference vs move) changed. Subscribers see the same data. |
| `static_cast<int32_t>(declare_parameter<int32_t>(...))`                                             | Values (port, udp_port, etc.) fit in int32_t. Same in practice.                                                       |
| `current_frame_id_ = static_cast<int64_t>(pkt->idx)`                                                | Frame IDs fit in int64_t. Same value.                                                                                 |
| Build script: `CMAKE_EXPORT_COMPILE_COMMANDS=ON`                                                    | Only affects tooling; does not change compiled binary.                                                                |

## 2. Timestamp and `rclcpp::Time` conversions

- **`rclcpp::Time(int64_t)`** expects **nanoseconds** (since epoch).
- All changes only made conversions **explicit**; the expressions and units are unchanged.

| Location                                        | Unit of input                                   | Expression                              | Effect                                                                                                                                                                                                                                                                       |
| ----------------------------------------------- | ----------------------------------------------- | --------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `publishPacket`: `timestamp * 1000`             | `timestamp` is **microseconds** (`ts_start_us`) | `timestamp * 1000` = nanoseconds        | Unchanged; cast is explicit only.                                                                                                                                                                                                                                            |
| `publishFrame` (fallback): `timestamp * 1000`   | Same (microseconds)                             | Same as above                           | Unchanged.                                                                                                                                                                                                                                                                   |
| `publishFrame` (first point): `point_timestamp` | **Seconds** (from `point.timestamp`)            | `static_cast<int64_t>(point_timestamp)` | **Same as before**: previously the code passed `rclcpp::Time(point_timestamp)` (double). The only matching constructor is `Time(int64_t)`, so the double was implicitly converted to int64_t (e.g. 1.5 → 1). We now do that conversion explicitly. So behavior is unchanged. |

**Pre-existing semantics:** For the “first point” branch, `point_timestamp` is in **seconds**, but it is passed to `rclcpp::Time(int64_t)` (nanoseconds). So a value like `1.5` (seconds) becomes `1` (nanosecond). That is a pre-existing unit mismatch; the recent edits did not introduce it and did not change the behavior.

## 3. Summary

- No change in intended behavior was introduced by the clang-tidy/pre-commit fixes.
- Timestamp handling is unchanged; the only nuance is the existing seconds-vs-nanoseconds semantics for `point_timestamp` in `publishFrame`, which is unchanged by these edits.
