-- Clean out stale records for fresh testing state
DELETE FROM AnalysisResults;

-- Reset SQLite autoincrement sequence counter
DELETE FROM sqlite_sequence WHERE name='AnalysisResults';

-- Insert bio-chemical surface vailidation logs
INSERT INTO AnalysisResults (session_id, substance_name, concentration, units, detection_limit, result_status, description) VALUES 
(101, 'Deionized Hydro-Purification Line A', 0.02, 'ppm', 0.05, 'Clear', 'Standard routine monitoring loop verification.'),
(101, 'Fluid Loop 1: Dissolved Oxygen Catalyst', 7.40, 'mg/L', 0.10, 'Optimal', 'Baseline operational validation checks safe.'),
(101, 'Aqueous Solvent Buffer (pH Stabilizer)', 6.85, 'pH',   0.01, 'Nominal', 'Buffer solution concentration parameters normal.');


-- --------------------------------------------------------------------
-- BATCH 002: PROCESS ANOMALIES & FLUID CONTAMINATION (CRITICAL VIOLATIONS)
-- --------------------------------------------------------------------
INSERT INTO AnalysisResults (session_id, substance_name, concentration, units, detection_limit, result_status, description) VALUES 
(102, 'Cooling Jacket B: Heavy Metal Dissolved Solids', 1.84, 'ppm', 0.10, 'Critical Over-Limit', 'Anomalous oxidation spikes discovered inside external coolant paths.'),
(102, 'Exhaust Scrub Vapor: Unrefined Sulfur Dioxide',  0.95, 'ppm', 0.05, 'Threshold Warning', 'Scrubber performance metrics drifting out of strict regulatory bounds.'),
(102, 'Secondary Line: Residual Ethylene Glycol Trace', 2.10, 'mg/L', 0.20, 'Critical Over-Limit', 'Potential cross-contamination containment leak detected inside backup tanks.');