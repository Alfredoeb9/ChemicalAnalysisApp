-- Clean out stale records for fresh testing state
DELETE FROM AnalysisResults;

-- Insert bio-chemical surface vailidation logs
INSERT INTO AnalysisResults (session_id, substance_name, concentration, units, detection_limits, result_status, description) VALUES
(1, 'ATP_Surface_Vent_A', 142.5, 'RLU', 10.0, 'Critical Failure', 'Excessive contamination detected'),
(1, 'ATP_Surface_Vent_B', 8.2, 'RLU', 10.0, 'Passed Clean', 'Within acceptable limits'),
(1, 'ATP_Fluid_Line_01', 45.0, 'RLU', 10.0, 'Warning Threshold', 'Slight deviation from baseline'),
(1, 'ATP_Control_Sample', 0.0, 'RLU', 10.0, 'Passed Clean', 'No contamination detected'),
(1, 'ATP_Handwash_Station', 215.3, 'RLU', 10.0, 'Critical Failure', 'Severe contamination detected');