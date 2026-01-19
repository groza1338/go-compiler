package main

import "fmt"

func main() {
    arr := [2][2]int{[2]int{1, 2}, [2]int{3, 4}}
    arr[1][0] = 9
    fmt.Print(arr[1][0])
    fmt.Print(" ")
    fmt.Print(arr[0][1])
}
