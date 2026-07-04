# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Dilshan R Jayakody. All rights reserved.

import os
import subprocess
import shutil

Import("env")


def run_parser_generators(env):
    print("\n[Bison/Flex] Generating parser and lexer files...")

    proj_dir = env.get("PROJECT_DIR")
    src_dir = os.path.join(proj_dir, "src")
    include_dir = os.path.join(proj_dir, "include")
    parser_dir = os.path.join(proj_dir, "parser")

    bison_file = os.path.join(proj_dir, "parser", "tinybasic.y")
    flex_file = os.path.join(proj_dir, "parser", "tinybasic.l")

    os.makedirs(src_dir, exist_ok=True)
    os.makedirs(include_dir, exist_ok=True)

    for f in [
        os.path.join(parser_dir, "tinybasic.tab.c"),
        os.path.join(src_dir, "tinybasic.tab.c"),
        os.path.join(src_dir, "lex.yy.c"),
        os.path.join(include_dir, "tinybasic.tab.h"),
    ]:
        if os.path.exists(f):
            os.remove(f)

    print("[Bison] Processing tinybasic.y...")

    subprocess.check_call([
        "bison",
        "-d",
        "-o",
        os.path.join(src_dir, "tinybasic.tab.c"),
        bison_file
    ])

    generated_header = os.path.join(src_dir, "tinybasic.tab.h")

    print("[Flex] Processing tinybasic.l...")

    subprocess.check_call([
        "flex",
        "-o",
        os.path.join(src_dir, "lex.yy.c"),
        flex_file
    ])

    if os.path.exists(generated_header):
        shutil.move(
            generated_header,
            os.path.join(include_dir, "tinybasic.tab.h")
        )

    print("[Bison/Flex] Target files successfully generated\n")


def clean_generated_files(env):
    print("\n[Clean] Removing generated parser and lexer files...")

    proj_dir = env.get("PROJECT_DIR")
    src_dir = os.path.join(proj_dir, "src")
    include_dir = os.path.join(proj_dir, "include")
    parser_dir = os.path.join(proj_dir, "parser")

    for f in [
        os.path.join(parser_dir, "tinybasic.tab.c"),
        os.path.join(src_dir, "tinybasic.tab.c"),
        os.path.join(src_dir, "lex.yy.c"),
        os.path.join(include_dir, "tinybasic.tab.h"),
    ]:
        if os.path.exists(f):
            os.remove(f)

    print("[Clean] Generated parser and lexer files removed\n")


if env.IsCleanTarget():
    clean_generated_files(env)
else:
    run_parser_generators(env)