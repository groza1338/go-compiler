package main

import "fmt"

func main() {
    arr := [2][3]int{[3]int{1, 2, 3}, [3]int{4, 5, 6}}
    fmt.Print(arr[0][1])
    fmt.Print(" ")
    fmt.Print(arr[1][2])
}
