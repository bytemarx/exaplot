#!/usr/bin/env bash

sizes=(16 32 64 128 256)

for size in "${sizes[@]}"; do
    inkscape --export-filename="icon-${size}.png" --export-type=png --export-width="${size}" --export-height="${size}" icon.svg
done

magick icon-16.png icon-32.png icon.ico
