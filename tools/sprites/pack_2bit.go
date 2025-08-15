package main

import (
	"fmt"
	"image"
	"image/color"
	_ "image/jpeg"
	_ "image/png"
	"log"
	"os"
	"path/filepath"
	"strings"
)

func getImageFromFilePath(filePath string) (image.Image, error) {
	f, err := os.Open(filePath)
	if err != nil {
		return nil, err
	}
	defer f.Close()
	image, _, err := image.Decode(f)
	return image, err
}

func colorToBits(c color.Color) string {
	r, g, b, a := c.RGBA()
	lightness := float64((r + g + b)) / 3.0 / 256.0

	// Transparent is 0
	if a < 128 {
		return "00"
	}

	if lightness < 85 {
		return "01"
	}

	if lightness < 170 {
		return "10"
	}

	return "11"
}

func main() {
	if len(os.Args) < 2 {
		log.Fatal("Missing argument: image path")
	}

	path := os.Args[1]

	img, err := getImageFromFilePath(path)

	if err != nil {
		log.Fatal(err)
	}

	width, height := img.Bounds().Dx(), img.Bounds().Dy()

	if (width*height)%4 != 0 {
		log.Fatal("Total image size must be a multiple of 4")
	}

	name := strings.TrimSuffix(path, filepath.Ext(path))

	if width == height {
		fmt.Printf("const size_t spr_%s_size = %d;\n", name, height)
	} else {
		fmt.Printf("const size_t spr_%s_width = %d;\n", name, width)
		fmt.Printf("const size_t spr_%s_height = %d;\n", name, height)
	}

	fmt.Printf("static uint8_t spr_%s_data[] = {\n", name)

	current_word := "0b"
	for y := 0; y < height; y++ {
		fmt.Print("    ")

		for x := 0; x < width; x++ {
			pixel := colorToBits(img.At(int(x), int(y)))
			current_word += pixel

			// reached uint8 ("0b" + 8 bits)
			if len(current_word) >= 10 {
				fmt.Print(current_word, ",")
				current_word = "0b"
			}
		}

		fmt.Println()
	}

	fmt.Println("};")
}
