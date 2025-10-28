void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("Enter calculation: ");
  
  while (Serial.available() == 0) {
    delay(100);
  }
  
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  int space1 = input.indexOf(' ');
  int space2 = input.indexOf(' ', space1 + 1);
  
  if (space1 != -1 && space2 != -1) {
    float num1 = input.substring(0, space1).toFloat();
    float num2 = input.substring(space2 + 1).toFloat();
    char op = input.charAt(space1 + 1);
    
    // Simulate sscanf success
    Serial.print("Debug: Successfully parsed ");
    Serial.print(num1);
    Serial.print(" ");
    Serial.print(op);
    Serial.print(" ");
    Serial.println(num2);
    
    float result;
    switch (op) {
      case '+': result = num1 + num2; break;
      case '-': result = num1 - num2; break;
      case '*': result = num1 * num2; break;
      case '/': result = num1 / num2; break;
      default: 
        Serial.println("Error: Invalid operator");
        return;
    }
    
    Serial.print("Result: ");
    Serial.println(result);
    
  } else {
    Serial.println("Error: Invalid format. Use: 8 + 5");
  }
  
  Serial.println();
}