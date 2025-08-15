package main

import (
	"fmt"
	"image"
	"image/color"
	_ "image/jpeg"
	_ "image/png"
	"log"
	"os"
)

// Size of letters
const FontWidth = 5
const FontHeight = 7

// Number of pixels between letters in sprite atlas
const Padding = 1

// Space is first (not included)
const Ordering = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-/|<>"

func getImageFromFilePath(filePath string) (image.Image, error) {
	f, err := os.Open(filePath)
	if err != nil {
		return nil, err
	}
	defer f.Close()
	image, _, err := image.Decode(f)
	return image, err
}

func colorToBit(c color.Color) string {
	r, g, b, _ := c.RGBA()
	lightness := float64((r+g+b)/3.0) / 256.0

	if lightness > 0.5 {
		return "0"
	} else {
		return "1"
	}
}

func main() {
	if len(os.Args) < 2 {
		log.Fatal("Missing argument: image path")
	}

	img, err := getImageFromFilePath(os.Args[1])

	if err != nil {
		log.Fatal(err)
	}

	width, height := img.Bounds().Dx(), img.Bounds().Dy()

	tilesX := width / (FontWidth + Padding)
	tilesY := height / (FontHeight + Padding)

	fmt.Println("uint8_t system_font[][LETTER_HEIGHT] = {")
	fmt.Println("    {0, 0, 0, 0, 0, 0 ,0}, // Space")

	// Loop over atlas
	index := 0
	for ty := 0; ty < tilesY && index < len(Ordering); ty++ {
		for tx := 0; tx < tilesX && index < len(Ordering); tx++ {
			// Convert letter
			minY := ty * (FontHeight + Padding)
			maxY := minY + FontHeight

			minX := tx * (FontWidth + Padding)
			maxX := minX + FontWidth

			fmt.Print("    {")

			// Print character data
			for y := minY; y < maxY; y++ {
				fmt.Print("0b")
				for x := minX; x < maxX; x++ {
					fmt.Print(colorToBit(img.At(int(x), int(y))))
				}
				fmt.Print(",")
			}

			fmt.Printf("}, // %s\n", string(Ordering[index]))

			index++
		}
	}

	fmt.Println("};")
}
