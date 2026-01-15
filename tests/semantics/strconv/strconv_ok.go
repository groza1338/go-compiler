package main

import "strconv"

func main() {
    i, _ := strconv.Atoi("123")
    f, _ := strconv.ParseFloat("1.5", 64)
    b, _ := strconv.ParseBool("true")
    _ = strconv.Itoa(i)
    _ = strconv.FormatBool(b)
    _ = strconv.FormatFloat(f, 1, 2, 64)
}
