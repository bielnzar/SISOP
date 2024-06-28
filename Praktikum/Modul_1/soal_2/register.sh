# Print welcome message
echo "Welcome to registration system"

# Function to check if a string is empty
is_empty() {
    if [ -z "$1" ]; then
        return 0   # empty
    else
        return 1   # not empty
    fi
}

# Function to check if email is valid
is_valid_email() {
    local email_regex="^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$"
    if [[ $1 =~ $email_regex ]]; then
        return 0   # valid
    else
        return 1   # invalid
    fi
}

# Function to check if password meets security requirements
is_valid_password() {
    local password="$1"
    local length=${#password}
    local has_uppercase=$(echo "$password" | grep -q '[[:upper:]]' && echo true || echo false)
    local has_lowercase=$(echo "$password" | grep -q '[[:lower:]]' && echo true || echo false)
    local has_digit=$(echo "$password" | grep -q '[[:digit:]]' && echo true || echo false)

    if [ $length -ge 8 ] && [ "$has_uppercase" == true ] && [ "$has_lowercase" == true ] && [ "$has_digit" == true ]; then
        return 0   # valid
    else
        return 1   # invalid
    fi
}

# Function to check if user exists in users.txt
user_exists() {
    local email="$1"
    if grep -q "^$email:" users.txt; then
        return 0   # exists
    else
        return 1   # does not exist
    fi
}

# Function to log messages to auth.log
log_message() {
    local type="$1"
    local message="$2"
    local date_time=$(date +"%d/%m/%y %T")
    echo "[$date_time] [$type] $message" >> auth.log
}

echo "Enter your email: "
read email
echo "Enter your username: "
read username
echo "Enter a security question: "
read security_question
echo "Enter the answer to your security question: "
read security_answer
echo "Enter a password (minimum 8 characters, at least 1 uppercase letter, 1 lowercase letter, 1 digit, 1 symbol, and not same as username, birthdate, or name) : "
read -s password

if is_empty "$email" || is_empty "$username" || is_empty "$security_question" || is_empty "$security_answer" || is_empty "$password"; then
    log_message "REGISTER FAILED" "ERROR Empty fields in registration form"
    echo "Error: Please fill in all fields."
    exit 1
fi

# Check if email is valid
if ! is_valid_email "$email"; then
    log_message "REGISTER FAILED" "ERROR Invalid email format for user registration"
    echo "Error: Invalid email format."
    exit 1
fi

# Check if password meets security requirements
if ! is_valid_password "$password"; then
    log_message "REGISTER FAILED" "ERROR Password does not meet security requirements"
    echo "Error: Password must be at least 8 characters long, contain at least one uppercase letter, one lowercase letter, and one digit."
    exit 1
fi

# Check if user already exists
if user_exists "$email"; then
    log_message "REGISTER FAILED" "ERROR User with email $email already exists"
    echo "Error: User with email $email already exists."
    exit 1
fi

# Encrypt password
encrypted_password=$(echo -n "$password" | base64)

# Append user data to users.txt
echo "$email:$username:$security_question:$security_answer:$encrypted_password" >> users.txt

log_message "REGISTER SUCCESS" "User with email $email registered successfully"
echo "Registration successful!"
