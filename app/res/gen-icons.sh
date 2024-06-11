#!/usr/bin/env bash

SIZES=(16 24 32 48 256)
FILES=()

for size in "${SIZES[@]}"; do
    inkscape --export-filename="icon-${size}.png" --export-type=png --export-width="${size}" --export-height="${size}" icon.svg
    FILES+=("icon-${size}.png")
done

magick ${FILES[@]} icon.ico

inkscape --export-filename=logo.png --export-type=png --export-width=128 --export-height=128 logo.svg
