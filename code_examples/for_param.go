package main

import "fmt"

func main() {
    sum := 0
    for i := 1; i <= 4; i += 1 {
        sum = sum + i
    }
    fmt.Print(sum)
}
