# ShadowStrike: Hacker's Fun Reporter

## Overview
This program is a command-line tool written in C that facilitates reporting abusive or spammy content on Facebook accounts using the Facebook Graph API. It allows users to report accounts by providing the necessary credentials and reporting details via a prompt-based interface.




## Features
- Reporting Functionality: Enables reporting of Facebook accounts by interactively specifying account ID, access token, reason, and category for the report.
- Dynamic Reporting Options: Users can input custom reasons and categories for reporting, allowing flexibility in reporting different types of abusive content.
- Error Handling and Retry Mechanism: Includes error recovery and resilience features with a retry mechanism to handle potential failures or rate limiting issues during reporting.
- Logging Functionality: Logs important messages and events during the reporting process to a log file (facebook_report.log) for reference and troubleshooting.
- Cross-Platform Compatibility: Developed to be compatible across different platforms, leveraging standard C libraries and the libcurl library for HTTP requests.



### Getting Started
-------
## Prerequisites
To compile and run this program, ensure you have the following installed:
- C Compiler (e.g., GCC)
- libcurl library

## Compilation
Use the following command to compile the program:

```bash
gcc -o facebook_report facebook_report.c -lcurl
```

## Usage
Run the compiled program: `./facebook_report`

# Follow the prompts to input the required details:
- Facebook account ID to report
- Facebook Graph API access token
- Reason for reporting
- Category for reporting

Example
```bash
Enter the Facebook account ID you want to report: [Enter Account ID]
Enter your Facebook Graph API access token: [Enter Access Token]
Enter the reason for reporting: [Enter Reason]
Enter the category for reporting: [Enter Category]
```

## Contributing
Contributions are welcome! Feel free to submit bug reports, suggestions, or pull requests.

## License
This project is licensed under the GNU GPL V3 License.


Feel the power, embrace the mystery, and let ShadowStrike be your weapon of choice in the hacker's playground!


