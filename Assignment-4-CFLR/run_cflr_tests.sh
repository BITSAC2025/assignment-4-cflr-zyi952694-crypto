#!/bin/bash
# 自动执行 .c → .bc → .dot → .txt → CFLR 的完整流程（适配 SVF 3.2）
# 修正版：用 dot_to_txt.py 替代原 awk 解析

SVF_DIR="/home/zyl94/SVF/Release-build"
CFLR_BIN="/home/zyl94/assignment-4-cflr-zyi952694-crypto/Assignment-4-CFLR/cflr"
TEST_DIR="/home/zyl94/assignment-4-cflr-zyi952694-crypto/Assignment-4-CFLR/Test-Cases"
RESULT_DIR="$TEST_DIR/results"

mkdir -p "$RESULT_DIR"

echo "=========================================="
echo "开始执行 .c → .bc → .dot → .txt → CFLR 测试流程"
echo "SVF_DIR:  $SVF_DIR"
echo "CFLR_BIN: $CFLR_BIN"
echo "=========================================="

# Python 转换脚本路径
DOT2TXT_PY="$TEST_DIR/dot_to_txt.py"

# 如果不存在则自动创建转换脚本
if [ ! -f "$DOT2TXT_PY" ]; then
    cat > "$DOT2TXT_PY" <<'EOF'
#!/usr/bin/env python3
import sys, re

if len(sys.argv) < 3:
    print("用法: python3 dot_to_txt.py input.dot output.txt")
    sys.exit(1)

node_id_map = {}
next_id = 0
edges = []

with open(sys.argv[1]) as f:
    for line in f:
        # 匹配 Node0x... -> Node0x... [color=xxx]
        m = re.search(r'(Node0x[0-9a-fA-F]+)\s*->\s*(Node0x[0-9a-fA-F]+)\[color=(\w+)\]', line)
        if m:
            src, dst, label = m.groups()
            if src not in node_id_map:
                node_id_map[src] = next_id
                next_id += 1
            if dst not in node_id_map:
                node_id_map[dst] = next_id
                next_id += 1
            edges.append((node_id_map[src], node_id_map[dst], label))

with open(sys.argv[2], "w") as out:
    for src, dst, label in edges:
        out.write(f"{src} {dst} {label}\n")

print(f"[dot2txt] 转换完成: {len(edges)} 条边, {len(node_id_map)} 个节点 → {sys.argv[2]}")
EOF
    chmod +x "$DOT2TXT_PY"
fi

for c_file in "$TEST_DIR"/*.c; do
    [ -f "$c_file" ] || continue
    base_name=$(basename "$c_file" .c)
    bc_file="$TEST_DIR/$base_name.bc"
    dot_file="$TEST_DIR/$base_name.dot"
    txt_file="$RESULT_DIR/$base_name.txt"
    cflr_out="$RESULT_DIR/$base_name.cflr"

    echo "------------------------------------------"
    echo "处理文件: $base_name.c"

    # Step 1: 编译 C → LLVM bitcode
    clang-16 -emit-llvm -c "$c_file" -o "$bc_file" 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "编译失败：$base_name.c"
        continue
    fi

    # Step 2: SVF 生成 .dot 图
    "$SVF_DIR/bin/wpa" -ander -dump-pag "$bc_file" >/dev/null 2>&1
    if [ ! -f "svfir_initial.dot" ]; then
        echo "未生成 svfir_initial.dot，跳过：$base_name"
        continue
    fi
    mv -f "svfir_initial.dot" "$dot_file"

    # Step 3: 调用 Python 脚本 .dot → .txt
    python3 "$DOT2TXT_PY" "$dot_file" "$txt_file"

    if [ ! -s "$txt_file" ]; then
        echo "警告：$base_name.txt 文件为空，跳过 CFLR。"
        continue
    fi

    # Step 4: 执行 CFLR 程序
    "$CFLR_BIN" "$txt_file" > "$cflr_out"
    echo "CFLR 结果输出至：$cflr_out"
done

echo "=========================================="
echo "所有测试完成！结果已保存至：$RESULT_DIR"
echo "=========================================="
