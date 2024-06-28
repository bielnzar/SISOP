
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

# Function to retrieve stored password from users.txt and decrypt it
get_stored_password() {
    local email="$1"
    local stored_password=$(grep "^$email:" users.txt | cut -d ':' -f 5)
    echo "$stored_password"
}

# Function to decrypt the password
decrypt_password() {
    local encrypted_password="$1"
    echo "$encrypted_password" | base64 -d
}

# Function to log messages to auth.log
log_message() {
    local type="$1"
    local message="$2"
    local date_time=$(date +"%d/%m/%y %T")
    echo "[$date_time] [$type] $message" >> auth.log
}

echo "Choose an option:"
echo "1. Login"
echo "2. Forgot Password"

read choice

if [ "$choice" == "1" ]; then
    echo "Enter your email: "
    read email
    echo "Enter your password: "
    read -s password

    if is_empty "$email" || is_empty "$password"; then
        log_message "LOGIN FAILED" "ERROR Empty email or password"
        echo "Error: Email and password cannot be empty."
        exit 1
    fi

    # Check if user exists
    if user_exists "$email"; then
        stored_password=$(get_stored_password "$email")
        decrypted_stored_password=$(decrypt_password "$stored_password")
        if [ "$password" == "$decrypted_stored_password" ]; then
            log_message "LOGIN SUCCESS" "User with email $email logged in successfully"
            echo "Login successful!"
            
            if grep -q "^$email:.*admin" users.txt; then
                # Admin actions
                echo "Admin actions:"
                echo "1. Add User"
                echo "2. Edit User"
                echo "3. Delete User"
                echo "4. Logout"
                read admin_action

                case "$admin_action" in
                    "1")  # Add User
                        echo "Enter new user's email: "
                        read new_email
                        echo "Enter new user's username: "
                        read new_username
                        echo "Enter new user's security question: "
                        read new_security_question
                        echo "Enter new user's security answer: "
                        read new_security_answer
                        echo "Enter new user's password: "
                        read -s new_password

                        if is_empty "$new_email" || is_empty "$new_username" || is_empty "$new_security_question" || is_empty "$new_security_answer" || is_empty "$new_password"; then
                            log_message "ADD USER FAILED" "ERROR Empty fields in new user registration form"
                            echo "Error: Please fill in all fields for the new user."
                            exit 1
                        fi

                        # Check if email is valid
                        if ! is_valid_email "$new_email"; then
                            log_message "ADD USER FAILED" "ERROR Invalid email format for new user registration"
                            echo "Error: Invalid email format for the new user."
                            exit 1
                        fi

                        # Check if password meets security requirements
                        if ! is_valid_password "$new_password"; then
                            log_message "ADD USER FAILED" "ERROR Password does not meet security requirements for new user"
                            echo "Error: Password must be at least 8 characters long, contain at least one uppercase letter, one lowercase letter, and one digit for the new user."
                            exit 1
                        fi

                        # Check if user already exists
                        if user_exists "$new_email"; then
                            log_message "ADD USER FAILED" "ERROR User with email $new_email already exists"
                            echo "Error: User with email $new_email already exists."
                            exit 1
                        fi

                        # Encrypt password
                        encrypted_password=$(echo -n "$new_password" | base64)

                        # Add new user
                        echo "$new_email:$new_username:$new_security_question:$new_security_answer:$encrypted_password" >> users.txt
                        log_message "ADD USER SUCCESS" "New user with email $new_email added successfully"
                        echo "New user with email $new_email added successfully."
                        ;;
                    "2")  # Edit User
                        echo "Enter user's email to edit: "
                        read edit_email
                        if ! user_exists "$edit_email"; then
                            log_message "EDIT USER FAILED" "ERROR User with email $edit_email does not exist"
                            echo "Error: User with email $edit_email does not exist."
                            exit 1
                        fi

                        echo "Enter new username: "
                        read new_username
                        echo "Enter new security question: "
                        read new_security_question
                        echo "Enter new security answer: "
                        read new_security_answer
                        echo "Enter new password: "
                        read -s new_password

                        if is_empty "$new_username" || is_empty "$new_security_question" || is_empty "$new_security_answer" || is_empty "$new_password"; then
                            log_message "EDIT USER FAILED" "ERROR Empty fields in user editing form"
                            echo "Error: Please fill in all fields for the user editing."
                            exit 1
                        fi

                        # Encrypt password
                        encrypted_password=$(echo -n "$new_password" | base64)

                        # Edit user details
                        sed -i "s/^$edit_email:.*/$edit_email:$new_username:$new_security_question:$new_security_answer:$encrypted_password/" users.txt
                        log_message "EDIT USER SUCCESS" "User with email $edit_email edited successfully"
                        echo "User with email $edit_email edited successfully."
                        ;;
                    "3")  # Delete User
                        echo "Enter user's email to delete: "
                        read delete_email
                        if ! user_exists "$delete_email"; then
                            log_message "DELETE USER FAILED" "ERROR User with email $delete_email does not exist"
                            echo "Error: User with email $delete_email does not exist."
                            exit 1
                        fi

                        # Delete user
                        sed -i "/^$delete_email:/d" users.txt
                        log_message "DELETE USER SUCCESS" "User with email $delete_email deleted successfully"
                        echo "User with email $delete_email deleted successfully."
                        ;;
                    "4")  # Logout
                        log_message "LOGOUT" "Admin logged out"
                        echo "Admin logged out."
                        ;;
                    *)    echo "Invalid admin action."
                          exit 1
                          ;;
                esac
            fi
        else
            log_message "LOGIN FAILED" "ERROR Incorrect password for user with email $email"
            echo "Error: Incorrect email or password."
            exit 1
        fi
    else
        log_message "LOGIN FAILED" "ERROR User with email $email does not exist"
        echo "Error: User with email $email does not exist."
        exit 1
    fi
elif [ "$choice" == "2" ]; then
    # Password recovery functionality
    echo "Enter your email: "
    read email

    if is_empty "$email"; then
        log_message "PASSWORD RECOVERY FAILED" "ERROR Empty email"
        echo "Error: Email cannot be empty."
        exit 1
    fi

    if user_exists "$email"; then
        # Security question verification
        stored_security_question=$(grep "^$email:" users.txt | cut -d ':' -f 3)
        echo "Security Question: $stored_security_question"
        echo "Enter your answer: "
        read security_answer

        stored_security_answer=$(grep "^$email:" users.txt | cut -d ':' -f 4)
        if [ "$security_answer" == "$stored_security_answer" ]; then
            stored_password=$(get_stored_password "$email")
            decrypted_stored_password=$(decrypt_password "$stored_password")
            echo "Your password is: $decrypted_stored_password"
            log_message "PASSWORD RECOVERY SUCCESS" "Password for user with email $email recovered successfully"
            exit 0
        else
            log_message "PASSWORD RECOVERY FAILED" "ERROR Incorrect security answer for user with email $email"
            echo "Error: Incorrect security answer."
            exit 1
        fi
    else
        log_message "PASSWORD RECOVERY FAILED" "ERROR User with email $email does not exist"
        echo "Error: User with email $email does not exist."
        exit 1
    fi
else
    echo "Invalid choice. Please choose 1 or 2."
    exit 1
fi
