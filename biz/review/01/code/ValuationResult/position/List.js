import { DataGrid, Card } from 'rootnet-ui'
import { DownCsv } from '../../../../common/components'
import { useGetColumns, useInitFetchPositon } from '../widgets/hooks'
import _ from 'lodash'

function PositionCard({ search, params, pageMode }) {
  const columns = useGetColumns('position')
  const { data, loading, error } = useInitFetchPositon(params, pageMode)
  const extra = <DownCsv tit='持仓价值' value={data} options={_.get(columns, 'columns', [])} label text='下载' />

  return (
    <Card title='持仓价值' className='flex-y x-card-singlegrid' extra={extra} loading={loading} error={error}>
      <DataGrid option={columns} data={data} className='row-fill hl_header' />
    </Card>
  )
}

export default PositionCard
