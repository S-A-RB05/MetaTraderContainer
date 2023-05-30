import pandas as pd

# Read the HTML file
html_file = 'Report/tester_report.htm'
df = pd.read_html(html_file)[0]  # Assuming the data is in the first table

# Save as CSV
csv_file = 'Report/output.csv'
df.to_csv(csv_file, index=False)

print("HTML to CSV conversion complete!")
