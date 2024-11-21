import googlemaps
import geocoder
from geopy.distance import geodesic
import socket
import time

esp_ip = '192.168.111.86'  # Replace with the actual IP address of your ESP8266
esp_port = 80

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((esp_ip, esp_port))

gmaps = googlemaps.Client(key='AIzaSyDaoK0lrW_hvu483JKGljFQtHKTzHSsLAk')

# List of hospitals with their coordinates
hospitals = [
    {"name": "Kamakshi Hospital", "coords": (12.949393168255757, 80.20953977020578)},
    {"name": "Medavakkam Primary Health Care", "coords": (12.914286638475092, 80.19278376243761)},
    {"name": "Prashanth Hospital", "coords": (12.97876748499598, 80.22132815655316)},
    {"name": "Dr. KAMAKSHI MULTISPECIALITY CLINIC", "coords": (12.918809115560208, 80.18758206313578)},
    {"name": "V Cure Hospital", "coords": (12.9280465865493, 80.19957678964725)},
    {"name": "Pallavaram Primary Health Care", "coords": (12.959884226976566, 80.16861584954209)},
    {"name": "Sringeri Sharada Equitas Hospital Multispecialty & Cancer care", "coords": (12.920903129751027, 80.1668591666146)},
    {"name": "New Life Hospital", "coords": (12.922623736747047, 80.15223950052429)},
    {"name": "Kalaignar Centenary Super Speciality Hospital KCSSH", "coords": (13.01514701240073, 80.21553866914654)}
]

# User input for accident location
latitude = float(input("Enter latitude of the accident location: "))
longitude = float(input("Enter longitude of the accident location: "))

# Store the accident location
accident_location = (latitude, longitude)

# Print accident location with full precision
print(f"Accident location: {accident_location[0]:.15f}, {accident_location[1]:.15f}")

nearest_hospital = None
shortest_route_distance = float('inf')  # Start with infinity

# Variable to store the waypoints
route_waypoints = []

# Iterate over each hospital
for hospital in hospitals:
    # Get directions from the accident location to the hospital
    directions = gmaps.directions(accident_location, hospital["coords"], mode="driving")

    if directions:
        # Extract route distance in meters
        route_distance = directions[0]["legs"][0]["distance"]["value"]

        # Check if this route distance is less than or equal to 5 km
        if route_distance <= 5000:  # 5 km in meters
            # If this route distance is shorter than the shortest found so far
            if route_distance < shortest_route_distance:
                shortest_route_distance = route_distance
                nearest_hospital = hospital
                # Store waypoints of the shortest route along with maneuvers
                route_waypoints = [
                    {
                        "location": (step["end_location"]["lat"], step["end_location"]["lng"]),
                        "maneuver": step.get("maneuver", "straight")  # Default to "straight" if no maneuver is provided
                    }
                    for step in directions[0]["legs"][0]["steps"]
                ]

if nearest_hospital:
    print(f"Nearest hospital: {nearest_hospital['name']}, Route distance: {shortest_route_distance / 1000:.2f} Kms (driving)")

    # Generate Google Maps URL for navigation
    origin = f"{accident_location[0]},{accident_location[1]}"
    destination = f"{nearest_hospital['coords'][0]},{nearest_hospital['coords'][1]}"
    google_maps_url = f"https://www.google.com/maps/dir/?api=1&origin={origin}&destination={destination}&travelmode=driving"
    print(f"Google Maps URL for navigation: {google_maps_url}")
    
    directions_string = ""
    # Print the list of waypoints with directions
    print("Waypoints along the route:")
    for waypoint in route_waypoints:
        # Format the maneuver
        direction = waypoint["maneuver"].lower()
        
        if "straight" in direction or "merge" in direction:
            direction = 'S'  # For straight or merge, use 'S'
        elif "right" in direction:
            direction = 'R'  # For right turns, use 'R'
        elif "left" in direction:
            direction = 'L'  # For left turns, use 'L'
        else:
            direction = 'S'  # Default to 'S' for any other maneuver

        directions_string += direction
        print(f"Waypoint: {waypoint['location']}, Direction: {direction}")

   
    for char in directions_string:
        client_socket.sendall(char.encode())  # Send each character as bytes
        time.sleep(0.5)

    
else:
    print("No hospital found within a 5 km driving distance.")

client_socket.close()

