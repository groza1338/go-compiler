package main

import "fmt"

func hi(a bool, s string) bool {
    str := "func_print: " + s
    fmt.Println(str)
    return a
}

func main() {
    var number int
    fmt.Scan(&number)

    if number > 10 && hi(true, "number > 10 && hi(true)") {
        fmt.Println("branch_print: number > 10 && hi(true)")
        fmt.Println()
    }

    if number > 5 && hi(true, "number > 5 && hi(true)") {
        fmt.Println("branch_print: number > 5 && hi(true)")
        fmt.Println()

    }

    if number > 10 || hi(true, "number > 10 || hi(true)") {
        fmt.Println("branch_print: number > 10 || hi(true)")
        fmt.Println()

    }

    if number > 5 || hi(true, "number > 5 || hi(true)") {
        fmt.Println("branch_print: number > 5 || hi(true)")
        fmt.Println()

    }

    if number > 10 && hi(false, "number > 10 && hi(false)") {
        fmt.Println("branch_print: number > 10 && hi(false)")
        fmt.Println()

    }

    if number > 5 && hi(false, "number > 5 && hi(false)") {
        fmt.Println("branch_print: number > 5 && hi(false)")
        fmt.Println()

    }

    if number > 10 || hi(false, "number > 10 || hi(false)") {
        fmt.Println("branch_print: number > 10 || hi(false)")
        fmt.Println()

    if number > 5 || hi(false, "number > 5 || hi(false)") {
        fmt.Println("branch_print: number > 5 || hi(false)")
        fmt.Println()
    }
}
}