# RPC 调用 rocksdb 请求协议设计

| index | 类型        | 作用         | 示例       |
| ----- | ----------- | ------------ | ---------- |
| 0     | uint8_t     | 标记调用方法 | 001 -> Put |
| 1     | json_string | options      | {}         |
| 1     | json_string | params       | {}         |

---------

| code | 方法 |
| ---- | ---- |
| 001  | Put  |
| 002  | Get  |
| 003  | Del  |