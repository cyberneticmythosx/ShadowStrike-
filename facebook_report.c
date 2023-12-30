#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <curl/curl.h>

#define API_VERSION "v12.0"
#define LOG_FILE "facebook_report.log"
#define RATE_LIMIT_DELAY 2

// Function to validate input
int validate_input(char *account_id, char *access_token) {
    if (account_id == NULL || access_token == NULL || strlen(account_id) == 0 || strlen(access_token) == 0) {
        fprintf(stderr, "Error: Both account ID and access token are required.\n");
        return 0;
    }
    return 1;
}

// Function to print banner
void print_banner() {
    printf("\033[91m  ██████  ██░ ██  ▄▄▄      ▓█████▄  ▒█████   █     █░     ██████ ▄▄▄█████▓ ██▀███   ██▓ ██ ▄█▀▓█████ \n");
    printf("\033[91m▒██    ▒ ▓██░ ██▒▒████▄    ▒██▀ ██▌▒██▒  ██▒▓█░ █ ░█░   ▒██    ▒ ▓  ██▒ ▓▒▓██ ▒ ██▒▓██▒ ██▄█▒ ▓█   ▀ \n");
    printf("\033[91m░ ▓██▄   ▒██▀▀██░▒██  ▀█▄  ░██   █▌▒██░  ██▒▒█░ █ ░█    ░ ▓██▄   ▒ ▓██░ ▒░▓██ ░▄█ ▒▒██▒▓███▄░ ▒███   \n");
    printf("\033[91m  ▒   ██▒░▓█ ░██ ░██▄▄▄▄██ ░▓█▄   ▌▒██   ██░░█░ █ ░█      ▒   ██▒░ ▓██▓ ░ ▒██▀▀█▄  ░██░▓██ █▄ ▒▓█  ▄ \n");
    printf("\033[91m▒██████▒▒░▓█▒░██▓ ▓█   ▓██▒░▒████▓ ░ ████▓▒░░░██▒██▓    ▒██████▒▒  ▒██▒ ░ ░██▓ ▒██▒░██░▒██▒ █▄░▒████▒\n");
    printf("\033[91m▒ ▒▓▒ ▒ ░ ▒ ░░▒░▒ ▒▒   ▓▒█░ ▒▒▓  ▒ ░ ▒░▒░▒░ ░ ▓░▒ ▒     ▒ ▒▓▒ ▒ ░  ▒ ░░   ░ ▒▓ ░▒▓░░▓  ▒ ▒▒ ▓▒░░ ▒░ ░\n");
    printf("\033[91m░ ░▒  ░ ░ ▒ ░▒░ ░  ▒   ▒▒ ░ ░ ▒  ▒   ░ ▒ ▒░   ▒ ░ ░     ░ ░▒  ░ ░    ░      ░▒ ░ ▒░ ▒ ░░ ░▒ ▒░ ░ ░  ░\n");
    printf("\033[91m░  ░  ░   ░  ░░ ░  ░   ▒    ░ ░  ░ ░ ░ ░ ▒    ░   ░     ░  ░  ░    ░        ░░   ░  ▒ ░░ ░░ ░    ░   \n");
    printf("\033[91m      ░   ░  ░  ░      ░  ░   ░        ░ ░      ░             ░              ░      ░  ░  ░      ░  ░\n");
    printf("                            ░\n");
    printf("\033[0m");
}

// Function to log messages to a file
void log_message(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log != NULL) {
        time_t now = time(NULL);
        fprintf(log, "[%s] %s\n", strtok(ctime(&now), "\n"), message);
        fclose(log);
    }
}

// Function to report Facebook account with dynamic options
void report_facebook_account(char *account_id, char *access_token, char *reason, char *category) {
    if (!validate_input(account_id, access_token)) {
        return;
    }

    char api_url[256];
    snprintf(api_url, sizeof(api_url), "https://graph.facebook.com/%s/%s/abuse_reports", API_VERSION, account_id);

    CURL *curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        char params[256];
        snprintf(params, sizeof(params), "access_token=%s&reason=%s&category=%s", access_token, reason, category);

        curl_easy_setopt(curl, CURLOPT_URL, api_url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params);

        CURLcode res;
        int retry_count = 3;
        do {
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                if (response_code == 200) {
                    printf("Success! Facebook account %s reported successfully.\n", account_id);
                    log_message("Reported account successfully");
                    break;
                } else {
                    fprintf(stderr, "Failed to report. Response: %s\n", curl_easy_strerror(res));
                    log_message("Failed to report account");
                }
            } else {
                fprintf(stderr, "Request error: %s\n", curl_easy_strerror(res));
                log_message("Request error");
            }

            // Delay before retrying
            if (--retry_count > 0) {
                log_message("Retrying...");
                sleep(RATE_LIMIT_DELAY);
            }
        } while (retry_count > 0);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        fprintf(stderr, "Failed to initialize libcurl.\n");
        log_message("Failed to initialize libcurl");
    }
}

int main() {
    print_banner();

    char account_id[256];
    char access_token[256];
    char reason[256];
    char category[256];

    printf("Enter the Facebook account ID you want to report: ");
    fgets(account_id, sizeof(account_id), stdin);
    account_id[strcspn(account_id, "\n")] = 0;

    printf("Enter your Facebook Graph API access token: ");
    fgets(access_token, sizeof(access_token), stdin);
    access_token[strcspn(access_token, "\n")] = 0;

    printf("Enter the reason for reporting: ");
    fgets(reason, sizeof(reason), stdin);
    reason[strcspn(reason, "\n")] = 0;

    printf("Enter the category for reporting: ");
    fgets(category, sizeof(category), stdin);
    category[strcspn(category, "\n")] = 0;

    // Enhanced user interface with configurable parameters
    printf("Reporting Facebook account %s with access token %s.\nReason: %s\nCategory: %s\n",
           account_id, access_token, reason, category);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    report_facebook_account(account_id, access_token, reason, category);

    curl_global_cleanup();

    return 0;
}
