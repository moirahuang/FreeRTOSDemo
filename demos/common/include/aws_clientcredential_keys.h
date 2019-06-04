/*
 * Amazon FreeRTOS V1.4.8
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUPIlslq4v0rhjDJozQAu3a7JfhaIwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDYwNDIwNDcz\n"\
"OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANEUMev9qLqD65R5cMLN\n"\
"68kHwjZfgP2DlBjidTefktXqte+3Pe7JRolFvgXiggsPgKa01quyGZz/fZF5Tecz\n"\
"Npz9+nbke7QSOUhQEnYtObLBAsJCHzvBwMRqemGH1375b9Jh4OEW4eNJ6Tr51hc0\n"\
"CnL1hTkS4EKndHlWOnuTzqlUTqVtt/0jIA8kNFxDt2zE3kcPljO3rbUldc0pwQsH\n"\
"aIeiifuFkVhrDbbM3PK9MrqjDQvV0/XV9hi/oMGCmULDbTkuO0ZkaBGD1SocWtXs\n"\
"vuwazEEDFFYqcM0+Da3ig6t8nUmr2dchbQh4muvQvmD/ZasHq5FuPraLfOQ615o1\n"\
"x2UCAwEAAaNgMF4wHwYDVR0jBBgwFoAU5FQnmO85hF5t1GaZ9Rj9LZTyfp0wHQYD\n"\
"VR0OBBYEFKyBBJWbI9aBbUtw7h8Wj8ygzZUWMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCAIJRujlsNwm4j5eIJ6PUQuwL1\n"\
"ywieI0olOG8CPoUuRbAIted/Sjw0Ra5E9OXgJrhdomqe62gfJG4CkEL234mXraFA\n"\
"gFB4N/PgBIk4BzeUj+Ms0kdYuTdE8ogM9C994LaBKVQlWJQyMMnwe5s/3CJgFCHg\n"\
"l9th29109NVPblN34HIoWqgBCPQ6C4fU0279g6rrhYuVq2zki7DOh7SMrjJJDxqe\n"\
"WLK06qXiCLolWJHs+Hy0Uwt/0EmXsrTbir4FCMq0T/kKvUrA8ihBTwyULpJ9U3jM\n"\
"pdk05iMoyEg8T+Qzzl1s0RA3jBXxri1mzu6yhXUImxfPScApELEzFFqz+v5/\n"\
"-----END CERTIFICATE-----\n"

/*
 * PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 * This is required if you're using JITR, since the issuer (Certificate 
 * Authority) of the client certificate is used by the server for routing the 
 * device's initial request. (The device client certificate must always be 
 * sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html, 
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 * 
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEA0RQx6/2ouoPrlHlwws3ryQfCNl+A/YOUGOJ1N5+S1eq177c9\n"\
"7slGiUW+BeKCCw+AprTWq7IZnP99kXlN5zM2nP36duR7tBI5SFASdi05ssECwkIf\n"\
"O8HAxGp6YYfXfvlv0mHg4Rbh40npOvnWFzQKcvWFORLgQqd0eVY6e5POqVROpW23\n"\
"/SMgDyQ0XEO3bMTeRw+WM7ettSV1zSnBCwdoh6KJ+4WRWGsNtszc8r0yuqMNC9XT\n"\
"9dX2GL+gwYKZQsNtOS47RmRoEYPVKhxa1ey+7BrMQQMUVipwzT4NreKDq3ydSavZ\n"\
"1yFtCHia69C+YP9lqwerkW4+tot85DrXmjXHZQIDAQABAoIBACmz7YE0x0LsrUCg\n"\
"OG2ixTUD5ykRQ5lNdWzBPsAphBBHEKMP2bqXSwp2bsCdvXKXVQHrcCYvEAzQqSsr\n"\
"Fu7WuD34Ks0sJcB1IhfaYNRRkQCmnvZAWhqBOwHyexaZEdKxziwYedJrzgKX631n\n"\
"99wI9UyYzEnmCfVBkWniWSftA3uiiQFjB/tvz9hxgn6CLFmByUzvDrifTEKlbROO\n"\
"EpKJmspsFzMoYnh+l0cqrkZl5PYGGc0E2B+9TlhI2FKhCjnEMYt+srIXSTOcTRK1\n"\
"TLIOJRXGP1KD/4M70sOVwQm0UjzeNiW0WWmXRgtOLp03U4GEjqLrEkNiOPO+Ekvl\n"\
"2u3j5AECgYEA6YV5rB8/+1ptK2G1z52OTC2yZBRxyzxCVbdaFh4y6kntPTc0uKfa\n"\
"E1MqwbEVshVNk+WdMB7vAvimPs32OqnmWZcrMX6Ihcaw5h45g6xeCVNVnX2YLg5U\n"\
"35LOwopUuGk5jdQ0iLEkmZdY9TPXQNy3N8XA+wOOBAYnaggSji5WPNUCgYEA5TRl\n"\
"608qDQsrsn8BoRQvTxGuH3wPlLm3Q1EgtAAEzM+XL/RD46Oy0sufznd3pvQUTThr\n"\
"7WKLzgqmsA/hCIepJ2ElOlVLNl4ks9gjZodY/PY8x4CcXiYVr1IU9euslLbQ8e2V\n"\
"akGMV+9RRj1XeoYBDDi/ibrowuAx0h3MqEpeaFECgYBwWeqHOGXFHeWGSLV7lRdA\n"\
"aX/5jKiPGofjy5pJJ/BhLn44xJXV/dP7FjltBTEmA//WF1QAMfHWoSEC+H7uDI4q\n"\
"LoP2hSVUD5tOq94KDuUTYYfAhQuxpO2Kv4xApuNJ8j5hkWZRGcE9MEb5eh6nM0nn\n"\
"SHqnijdKkldmOUoMJBkTnQKBgQC4JDytq9Aw4oScxwTcEIdhkua9TMKHzhy57HfF\n"\
"pS42DaF9WVUhkeadDBK7oadzbizRTT8IXF/NjSkV0ewNGlB/3sA1+UiPyVrbUg4+\n"\
"1QZdVMbH0T/PM/HABMXqPQlOAQIlG6cq0raZ/x7gu2+IdBhrZTwcvxk5Vz0vfdlq\n"\
"azUlwQKBgCcw9kKBAHZ2xC8FmCH7T1WLU3xSnVva/R+xzUL62aY+KpPfasVdogJu\n"\
"Qmj8BWwAZcLc3+t6xO3a3+nuywfUfS4jzzpGKsTk2UBlYwsWfdCVA05DQ+eDIYVM\n"\
"taYW217+TYzGpaxlEjd57ftYpp06x49iGbYgA2nnNT/p1RivRlgH\n"\
"-----END RSA PRIVATE KEY-----\n"

/* The constants above are set to const char * pointers defined in aws_demo_runner.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS 
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
