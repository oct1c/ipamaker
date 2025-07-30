//developed by Octic
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#define MAX_PATH 1024

int main(int argc, char *argv[]) {
    char *output_path = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "o:")) != -1) {
        if (opt == 'o') output_path = optarg;
        else {
            fprintf(stderr, "Usage: %s <.app_file_path> [-o output_path]\nDeveloped by: Octic\n", argv[0]);
            return 1;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Usage: %s <.app_file_path> [-o output_path]\nDeveloped by: Octic\n", argv[0]);
        return 1;
    }
    char input_path[MAX_PATH], ipa_path[MAX_PATH], abs_ipa_path[MAX_PATH];
    if (strchr(argv[optind], '/') == NULL) {
        snprintf(input_path, MAX_PATH, "./%s", argv[optind]);
    } else {
        strncpy(input_path, argv[optind], MAX_PATH - 1);
        input_path[MAX_PATH - 1] = '\0';
    }
    struct stat statbuf;
    if (stat(input_path, &statbuf) != 0) {
        perror("Error: Failed to access input path");
        fprintf(stderr, "Error: '%s' is not a valid .app directory\n", input_path);
        return 1;
    }
    if (!S_ISDIR(statbuf.st_mode)) {
        fprintf(stderr, "Error: '%s' is not a directory\n", input_path);
        return 1;
    }
    char temp_path[MAX_PATH], cmd[MAX_PATH * 2 + 50];
    strncpy(temp_path, input_path, MAX_PATH - 1);
    temp_path[MAX_PATH - 1] = '\0';
    char *base_name = basename(temp_path);
    char *dot = strrchr(base_name, '.');
    if (dot && strcmp(dot, ".app") != 0) {
        fprintf(stderr, "Error: '%s' does not end with .app\n", input_path);
        return 1;
    }
    if (dot) *dot = '\0';
    if (output_path) {
        if (stat(output_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            char *clean_output = output_path;
            size_t len = strlen(output_path);
            if (len > 0 && output_path[len - 1] == '/') clean_output[len - 1] = '\0';
            snprintf(ipa_path, MAX_PATH, "%s/%s.ipa", clean_output, base_name);
        } else {
            strncpy(ipa_path, output_path, MAX_PATH - 1);
            ipa_path[MAX_PATH - 1] = '\0';
        }
    } else {
        snprintf(ipa_path, MAX_PATH, "%s/%s.ipa", dirname(strdup(input_path)), base_name);
    }
    if (stat(ipa_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
        fprintf(stderr, "Error: Output path '%s' is a directory\n", ipa_path);
        return 1;
    }
    if (ipa_path[0] == '/') {
        strncpy(abs_ipa_path, ipa_path, MAX_PATH - 1);
        abs_ipa_path[MAX_PATH - 1] = '\0';
    } else {
        char *cwd = getcwd(NULL, 0);
        if (!cwd) {
            perror("Error: Failed to get current working directory");
            return 1;
        }
        snprintf(abs_ipa_path, MAX_PATH, "%s/%s", cwd, ipa_path);
        free(cwd);
    }
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", dirname(strdup(ipa_path)));
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: Failed to create output directory for '%s' (errno: %d)\n", ipa_path, errno);
        return 1;
    }
    snprintf(cmd, sizeof(cmd), "command -v zip >/dev/null");
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: 'zip' command not found. Please install zip.\n");
        return 1;
    }
    snprintf(cmd, sizeof(cmd), "mkdir -p /tmp/Payload");
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: Failed to create temporary Payload directory (errno: %d)\n", errno);
        return 1;
    }
    if (access("/tmp", W_OK) != 0) {
        perror("Error: Cannot write to /tmp");
        return 1;
    }
    snprintf(cmd, sizeof(cmd), "cp -r %s /tmp/Payload/", input_path);
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: Failed to copy '%s' to /tmp/Payload (errno: %d)\n", input_path, errno);
        return 1;
    }
    snprintf(cmd, sizeof(cmd), "cd /tmp && zip -r %s Payload >/dev/null", abs_ipa_path);
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: Failed to create .ipa file '%s' (errno: %d)\n", ipa_path, errno);
        return 1;
    }
    snprintf(cmd, sizeof(cmd), "rm -rf /tmp/Payload");
    system(cmd);
    printf("Successfully created %s\n", ipa_path);
    return 0;
}