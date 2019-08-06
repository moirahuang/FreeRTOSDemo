/*
 * Amazon FreeRTOS V201906.00 Major
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
"MIIDWjCCAkKgAwIBAgIVANK0tVvGIDnYt9uG7xh4EtD3pLmhMA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTA4MDYxODI2\n"\
"MDVaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDNoHZs7JgASYL/PSLb\n"\
"6xEQYlNgmogLuROWdB1W4IXa+n+iA7u6EsbRMBHwzrok9KBlryyd9DW6N2pXaBiq\n"\
"6nIoMrCKy5sjTr+OczA11zCLNa2kdnmF/phdcDvlSIICoHMFXkc/9SZncOKENYhu\n"\
"dscx5qlgT7u2ErjCGQpOeLGcaHjSXbvVFWh7nPRf5fQ1T99TCLPDBn3h72e4BlZS\n"\
"e/4gQXq0gItrC7h1PsnI0/J+vAsNy7MiYrSu+VNqJihYhNwO8fi1HiBAsQE/Qe9j\n"\
"h8XGHKFPm6WM2MBvnVTh23WEoj86+mnGHN0JYGL1OG1ocFrvK7dkZr6KHt8uWWP4\n"\
"CvoPAgMBAAGjYDBeMB8GA1UdIwQYMBaAFHmaBmhbsClgQ7X9yjICWnr3FrzHMB0G\n"\
"A1UdDgQWBBQYBlMCsjfgkNPhVtsjatPq1KX7ODAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAF1bnLLoZczz0GfbZ4K5MtZqW\n"\
"MmC83/Cy8/IelCzWYLAbrDTRDwOyfUkv45fZbwyZ/zKQ2oXAHVJE7s3iCGDnBuTG\n"\
"K3hY+L8V/pPfb1S74EkeuqGeZcWJl6sfZkQLBLbY6PEKmsmveC+V8AXYlCknwlQi\n"\
"AkWA/7+Ag/AgeXrNfsQBMddj+7lkj1Locr7/rge4q4Zh5Jf+w4RdsgjDgPJvnMy0\n"\
"3QWu7JHBbxrh+X/nOWX0+q2XjN4PwLqroB43WEqqfH2G9YxVNjd1s9Yf91Zw6y2z\n"\
"9epL6BEE0ZG1JuAAM++4YrGJW6trBAyexHjFR3JncMj6yGVDYoeiV6Uxltfq9w==\n"\
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
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  ""

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
"MIIEowIBAAKCAQEAzaB2bOyYAEmC/z0i2+sREGJTYJqIC7kTlnQdVuCF2vp/ogO7\n"\
"uhLG0TAR8M66JPSgZa8snfQ1ujdqV2gYqupyKDKwisubI06/jnMwNdcwizWtpHZ5\n"\
"hf6YXXA75UiCAqBzBV5HP/UmZ3DihDWIbnbHMeapYE+7thK4whkKTnixnGh40l27\n"\
"1RVoe5z0X+X0NU/fUwizwwZ94e9nuAZWUnv+IEF6tICLawu4dT7JyNPyfrwLDcuz\n"\
"ImK0rvlTaiYoWITcDvH4tR4gQLEBP0HvY4fFxhyhT5uljNjAb51U4dt1hKI/Ovpp\n"\
"xhzdCWBi9ThtaHBa7yu3ZGa+ih7fLllj+Ar6DwIDAQABAoIBAB6DSi99qEyam/q+\n"\
"GrchmaWhwZebNsbujs6LqnLi3FoTQ+9JuIizEpemF3NxacPkHQ0Zwngp7fMVGA2i\n"\
"X860/X2jj/j4n6vMmrZLt4K1zPlLXNoDzqKolfl/lxO9/7TeFFNemxoNicZdxO8z\n"\
"dmxV6dco0Ed/bW5mNs5uos3kMQ7xWZMtk77OWr/3wGdH1Ppyqk/MGZAO5ISKdUKf\n"\
"FAwvecxqvYa8+HUjn6El22TV+8+m7O3cwJTfayuxQAqg8oCaubq6/TjuTAMu8/U+\n"\
"aVogvZT3xt+EqdWuKZQcm1HWZrjVvfWFaZg7sNeHfeIk71N4yGRoWQb7BHUG8jJr\n"\
"Dmw0s5kCgYEA8jWIvLtuFYXRLuDVGWaJTjtlwYDQhsbt9Ckduc8iuXFL2I1rGftQ\n"\
"lHaRcTuk2rd+Zfnvmxtio7frJKYJGgh6GZ4uw1/3u8n73XhLds1FETlt/gWMeAcO\n"\
"s9bz6IIBv9Acr8lDR94AD4k0XgCNoVPu7kkJNHGezVFWN/u3NGzL0gsCgYEA2VWz\n"\
"aMEY7GTKa09O3fWPZe9t124voeIzBkkdc8G4+4F74wdMN89+yTWuaTJAUWkxQd5M\n"\
"ibQva/1+YdZva4LXVxRffp7X3qWJ6YRn/YjpctOQIfPBYaKI+D2g/QM5VG6emYuv\n"\
"8Warm6MsiPQRBuCDUsR+PSew3vppbwNSo5lhHo0CgYEAngQXBSoyk5AN/Mtpu3yN\n"\
"r6BIrolfgc5eh6AbaJGeUeV8WfdJHFY9J96C3KDQ2HBF4jnA/CxNmgnvOqWn4ngN\n"\
"UJFGaOg/MyB/VOmaMhORL7+L5uGv1s6j5OgjkFjg00busDBZ2JUY7QnSRFGQFOQx\n"\
"SPwVKUXHGKQr37I0v9sKzikCgYAWks8ypzPGgeWAZdlkNngxdz/qa1hpMQxpr7hu\n"\
"aIWlIdrTRtPEzDl3/qph5MVHb2ydUdCTUPLH/ald0h76hLmHIueUSD1Vs83yQj1m\n"\
"2QyeT5kd3aqNUIt3dTc+cFJJEcBJZHfe0/IpFTav9uG53CHnu978zR2S22GOFUNF\n"\
"GM48lQKBgFQNvyf87Wjc1uASKyep5XUIPzB3LP97aFnsQmG4KllrEF3s5P/F0XuV\n"\
"DRo96uxRv3ThN7wPFgcrihK/CWETgcCuSQv+ldcuG+tll1Smz5jQ2cUo/GVJmskk\n"\
"jxVd76JvS2CA4/g1jRp750NpxqdWQXUQ1ZkC6eOcxoIsomtcKjYr\n"\
"-----END RSA PRIVATE KEY-----\n"

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
