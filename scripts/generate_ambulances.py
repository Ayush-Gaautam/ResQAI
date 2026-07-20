import csv
import random

INPUT_FILE = "data/processed/hospitals.csv"
OUTPUT_FILE = "data/processed/ambulances.csv"

ambulances = []

with open(INPUT_FILE, "r", newline="", encoding="utf-8") as file:
    reader = csv.DictReader(file)

    count = 1

    for row in reader:

        ambulance_id = f"AMB{count:03d}"

        node_id = row["nodeId"]

        status = random.choices(
            ["Available", "Busy"],
            weights=[80, 20],
            k=1
        )[0]

        ambulances.append({
            "ambulanceId": ambulance_id,
            "nodeId": node_id,
            "status": status
        })

        count += 1

with open(OUTPUT_FILE, "w", newline="", encoding="utf-8") as file:

    writer = csv.DictWriter(
        file,
        fieldnames=[
            "ambulanceId",
            "nodeId",
            "status"
        ]
    )

    writer.writeheader()

    writer.writerows(ambulances)

print("--------------------------------")
print("Ambulances Generated Successfully")
print("Total Ambulances :", len(ambulances))
print("Saved to :", OUTPUT_FILE)
print("--------------------------------")