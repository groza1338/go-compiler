package main

import "fmt"

func multiFunc(x_1 int, x_2 float64, x_3 string, x_4 rune, x_5 bool) (int, float64, string, rune, bool) {
    return x_1 + 2, x_2 + 7.9, x_3 + "_new_part", x_4 + 2, !x_5
}

func main() {
    fmt.Print(multiFunc(50, 22.2, "my_string", 'a', true))
}