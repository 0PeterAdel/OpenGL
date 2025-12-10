#!/bin/bash

# ===============================
# 🌟 Fancy OpenGL Build Script 🌟
# ===============================

# Colors
GREEN="\e[32m"
CYAN="\e[36m"
YELLOW="\e[33m"
RED="\e[31m"
RESET="\e[0m"

clear
echo -e "${CYAN}"
echo "======================================="
echo "      🚀 Building OpenGL Project       "
echo "======================================="
echo -e "${RESET}"

echo -e "${YELLOW}🔧 Step 1: Detecting flag source files...${RESET}"

# Auto-detect all flag .cpp files
FLAGS=src/flags/*.cpp

echo -e "${CYAN}Found flag files:${RESET}"
for f in $FLAGS; do
    echo -e "   - ${GREEN}$f${RESET}"
done

echo
echo -e "${YELLOW}🔧 Step 2: Compiling source files...${RESET}"

g++ -std=c++17 \
src/main.cpp src/glad.c \
$FLAGS \
imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp \
imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp \
-Iinclude -Iimgui \
-lglfw -ldl -lGL -lpthread -o app

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Compilation Failed!${RESET}"
    exit 1
fi

echo -e "${GREEN}✔ Compilation Successful!${RESET}"

echo
echo -e "${YELLOW}🚀 Step 3: Running the application...${RESET}"
echo -e "${CYAN}---------------------------------------${RESET}"

sleep 1

./app

echo -e "${CYAN}---------------------------------------${RESET}"
echo -e "${GREEN}🎉 Program Finished Successfully!${RESET}"
