-- Simple calculator example
print("=== Simple Lua Calculator ===")
print()

local function add(a, b)
    return a + b
end

local function subtract(a, b)
    return a - b
end

local function multiply(a, b)
    return a * b
end

local function divide(a, b)
    if b == 0 then
        return "Error: Division by zero"
    end
    return a / b
end

-- Test calculations
print("10 + 5 = " .. add(10, 5))
print("10 - 5 = " .. subtract(10, 5))
print("10 * 5 = " .. multiply(10, 5))
print("10 / 5 = " .. divide(10, 5))
print("10 / 0 = " .. divide(10, 0))
