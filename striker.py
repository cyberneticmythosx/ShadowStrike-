import logging
import asyncio
import configparser

try:
    import requests
    import aiohttp
except ImportError as e:
    print(f"Warning: {e.name} module not found. You may need to install it.")
    install_choice = input("Do you want to install missing modules? (y/n): ").lower()
    if install_choice == 'y':
        try:
            from pip._internal import main as pipmain
        except ImportError:
            from pip import main as pipmain
        pipmain(['install', 'requests', 'aiohttp'])
    else:
        print("Please install the required modules manually.")
        exit()

# Load configuration from a file
config = configparser.ConfigParser()
config.read('config.ini')

# Set default values
API_VERSION = config.get('General', 'api_version', fallback='v12.0')
LOG_FILE = config.get('Logging', 'log_file', fallback='facebook_report.log')
RATE_LIMIT_DELAY = int(config.get('RateLimiting', 'delay_seconds', fallback=2))

async def validate_input(account_id, access_token):
    """
    Validate user input for account ID and access token.

    Parameters:
    - account_id (str): The Facebook account ID to report.
    - access_token (str): The Facebook Graph API access token.

    Returns:
    - bool: True if input is valid, False otherwise.
    """
    if not account_id or not access_token:
        logging.error("Error: Both account ID and access token are required.")
        return False
    return True

def print_banner():
    """
    Print the ASCII art banner.
    """
    print("""
\033[91m  ██████  ██░ ██  ▄▄▄      ▓█████▄  ▒█████   █     █░     ██████ ▄▄▄█████▓ ██▀███   ██▓ ██ ▄█▀▓█████ 
\033[91m▒██    ▒ ▓██░ ██▒▒████▄    ▒██▀ ██▌▒██▒  ██▒▓█░ █ ░█░   ▒██    ▒ ▓  ██▒ ▓▒▓██ ▒ ██▒▓██▒ ██▄█▒ ▓█   ▀ 
\033[91m░ ▓██▄   ▒██▀▀██░▒██  ▀█▄  ░██   █▌▒██░  ██▒▒█░ █ ░█    ░ ▓██▄   ▒ ▓██░ ▒░▓██ ░▄█ ▒▒██▒▓███▄░ ▒███   
\033[91m  ▒   ██▒░▓█ ░██ ░██▄▄▄▄██ ░▓█▄   ▌▒██   ██░░█░ █ ░█      ▒   ██▒░ ▓██▓ ░ ▒██▀▀█▄  ░██░▓██ █▄ ▒▓█  ▄ 
\033[91m▒██████▒▒░▓█▒░██▓ ▓█   ▓██▒░▒████▓ ░ ████▓▒░░░██▒██▓    ▒██████▒▒  ▒██▒ ░ ░██▓ ▒██▒░██░▒██▒ █▄░▒████▒
\033[91m▒ ▒▓▒ ▒ ░ ▒ ░░▒░▒ ▒▒   ▓▒█░ ▒▒▓  ▒ ░ ▒░▒░▒░ ░ ▓░▒ ▒     ▒ ▒▓▒ ▒ ░  ▒ ░░   ░ ▒▓ ░▒▓░░▓  ▒ ▒▒ ▓▒░░ ▒░ ░
\033[91m░ ░▒  ░ ░ ▒ ░▒░ ░  ▒   ▒▒ ░ ░ ▒  ▒   ░ ▒ ▒░   ▒ ░ ░     ░ ░▒  ░ ░    ░      ░▒ ░ ▒░ ▒ ░░ ░▒ ▒░ ░ ░  ░
\033[91m░  ░  ░   ░  ░░ ░  ░   ▒    ░ ░  ░ ░ ░ ░ ▒    ░   ░     ░  ░  ░    ░        ░░   ░  ▒ ░░ ░░ ░    ░   
\033[91m      ░   ░  ░  ░      ░  ░   ░        ░ ░      ░             ░              ░      ░  ░  ░      ░  ░
                            ░
\033[0m""")

async def report_facebook_account(account_id, access_token, reason="fake_account", category="fake_account"):
    """
    Report a Facebook account for abuse asynchronously.

    Parameters:
    - account_id (str): The Facebook account ID to report.
    - access_token (str): The Facebook Graph API access token.
    - reason (str): The reason for reporting (default: "fake_account").
    - category (str): The category of the report (default: "fake_account").

    Returns:
    - None
    """
    if not await validate_input(account_id, access_token):
        return

    api_url = f"https://graph.facebook.com/{API_VERSION}/{account_id}/abuse_reports"

    # Set up parameters for reporting
    params = {
        "access_token": access_token,
        "reason": reason,
        "category": category
    }

    try:
        # Make a POST request to report the account asynchronously
        async with aiohttp.ClientSession() as session:
            async with session.post(api_url, params=params) as response:
                # Check the response status
                if response.status == 200:
                    logging.info(f"Success! Facebook account {account_id} reported successfully.")
                else:
                    logging.error(f"Failed to report. Response: {await response.text()}")
    except aiohttp.ClientError as e:
        logging.error(f"Request error: {str(e)}")
    except Exception as e:
        logging.error(f"Error: {str(e)}")

async def rate_limiting_decorator(func):
    """
    A decorator for rate limiting to avoid exceeding API rate limits.
    """
    async def wrapper(*args, **kwargs):
        # Add rate-limiting logic here
        await asyncio.sleep(RATE_LIMIT_DELAY)
        return await func(*args, **kwargs)
    return wrapper

# Apply rate limiting to the reporting function
report_facebook_account = rate_limiting_decorator(report_facebook_account)

# Example usage
print_banner()
account_id = input("Enter the Facebook account ID you want to report: ")
access_token = input("Enter your Facebook Graph API access token: ")

# Configure logging with improved formatting
logging.basicConfig(filename=LOG_FILE, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Asynchronous event loop
loop = asyncio.get_event_loop()
loop.run_until_complete(report_facebook_account(account_id, access_token, reason="spam", category="spam_content"))
