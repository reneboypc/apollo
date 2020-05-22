/******************************************************************************
 * Copyright 2020 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 **/

#include "modules/planning/tasks/learning_model/learning_model_inference_trajectory_task.h"

#include <vector>

namespace apollo {
namespace planning {

using apollo::common::ErrorCode;
using apollo::common::Status;
using apollo::common::TrajectoryPoint;

LearningModelInferenceTrajectoryTask::LearningModelInferenceTrajectoryTask(
    const TaskConfig &config) : Task(config) {
}

Status LearningModelInferenceTrajectoryTask::Execute(
    Frame *frame,
    ReferenceLineInfo *reference_line_info) {
  CHECK_NOTNULL(frame);

  Task::Execute(frame, reference_line_info);
  return Process(frame);
}

Status LearningModelInferenceTrajectoryTask::Process(Frame *frame) {
  CHECK_NOTNULL(frame);

  WriteTrajectory(frame);

  return Status::OK();
}

bool LearningModelInferenceTrajectoryTask::WriteTrajectory(Frame* frame) {
  CHECK_NOTNULL(frame);

  auto reference_line_infos = frame->mutable_reference_line_infos();
  if (reference_line_infos->empty()) {
    AERROR << "no reference is found.";
    return false;
  }
  // FIXME(all): current only pick up the first reference line to use
  // learning model trajectory.
  for (auto& reference_line_info : *reference_line_infos) {
    reference_line_info.SetDrivable(false);
  }
  auto& picked_reference_line_info = reference_line_infos->front();
  picked_reference_line_info.SetDrivable(true);
  picked_reference_line_info.SetCost(0);

  std::vector<TrajectoryPoint> trajectory_points
      = frame->learning_data_adc_future_trajectory_points();

  picked_reference_line_info.SetTrajectory(
     DiscretizedTrajectory(trajectory_points));

  return true;
}

}  // namespace planning
}  // namespace apollo
