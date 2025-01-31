def add_numbers(num1, num2):
  """
  Adds two numbers together.

  Args:
    num1: The first number.
    num2: The second number.

  Returns:
    The sum of num1 and num2.
  """
  return num1 + num2

if __name__ == "__main__":
  # Get input from the user (for testing purposes)
  # num1 = int(input("Enter the first number: "))
  # num2 = int(input("Enter the second number: "))
  num1, num2 = 5, 6
  # Calculate the sum
  result = add_numbers(num1, num2)

  # Print the result
  print(f"The sum of {num1} and {num2} is: {result}")

  # Optional: Save the result to a file
  with open("result.txt", "w") as f:
    f.write(str(result))
