package main

import "fmt"

func main() {
    matrix := [2][3]int{{1, 2, 3}, {4, 5, 6}}
    fmt.Print(matrix[1][:2])
}
